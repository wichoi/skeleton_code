#include <stdlib.h>
#include <unistd.h>

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include "af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"

#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "stack/include/trust-center.h"

#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY
  #include "app/framework/plugin/network-creator-security/network-creator-security.h"
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR
  #include "app/framework/plugin/network-creator/network-creator.h"
#endif

#include "apro-common.h"
#include "apro-log.h"
#include "apro-config.h"
#include "apro-event.h"
#include "apro-proc.h"
#include "apro-handle.h"

int apro_handle_version(u8 *major, u8 *minor, u8 *patch, u16 *build)
{
    int ret_val = RET_ERROR;
    //emAfCliVersionCommand();
    EmberVersion versionStruct;
    uint8_t ncpEzspProtocolVer;
    uint8_t ncpStackType;
    uint16_t ncpStackVer;
    uint8_t hostEzspProtocolVer = EZSP_PROTOCOL_VERSION;
    ncpEzspProtocolVer = ezspVersion(hostEzspProtocolVer,
                                     &ncpStackType,
                                     &ncpStackVer);
    EzspStatus st = ezspGetVersionStruct(&versionStruct);
    if(st == EMBER_SUCCESS)
    {
        log_d("ezsp ver[0x%x] stack type[0x%x] stack ver[%d.%d.%d.%d]\n",
                        ncpEzspProtocolVer,
                        ncpStackType,
                        versionStruct.major,
                        versionStruct.minor,
                        versionStruct.patch,
                        versionStruct.build);
        
        *major = versionStruct.major;
        *minor = versionStruct.minor;
        *patch = versionStruct.patch;
        *build = versionStruct.build;
        ret_val = RET_SUCCESS;
    }
    else
    {
        log_i("%s error[0x%02x]\n", __func__, st);
    }

    return ret_val;
}

int apro_handle_eui64(void)
{
    EmberEUI64 eui64;
    emberAfGetEui64(eui64);
    log_d("%s\n", __func__);

    int i;
    for(i = 0 ; i < EUI64_SIZE; i++)
    {
        debug_printf("%x ", emLocalEui64[i]);
    }
    debug_printf("\n");
    return RET_SUCCESS;
}

int apro_handle_node_id(void)
{
    EmberNodeId id = emberAfGetNodeId();
    log_d("%s : 0x%04X\n", __func__, id);
    return RET_SUCCESS;
}

int apro_handle_pan_id(void)
{
    EmberPanId id = emberAfGetPanId();
    log_d("%s : 0x%04X\n", __func__, id);
    return RET_SUCCESS;
}

int apro_handle_channel(void)
{
    uint8_t ch = emberAfGetRadioChannel();
    log_d("%s : 0x%02X(%u)\n", __func__, ch, ch);
    return RET_SUCCESS;
}

int apro_handle_set_confing(u8 id, u16 val)
{
    int ret_val = RET_ERROR;
    EzspConfigId configId = (EzspConfigId)id;
    uint16_t value = (uint16_t)val;
    uint8_t configIdName[32] ={0,};
    //uint8_t length = emberCopyStringArgument(2, configIdName, 32, false);
    EzspStatus st = emberAfSetEzspConfigValue(configId,
                                              value,
                                              (const char *)configIdName);
    if(st == EMBER_SUCCESS)
    {
        log_d("%s[0x%02x]: id[0x%02x] value[0x%04x] %s\n", __func__,
                                        st, configId, value, configIdName);
        ret_val = RET_SUCCESS;
    }
    else
    {
        log_i("%s error[0x%02x]\n", __func__, st);
    }

    return ret_val;
}

int apro_handle_get_net_st(u8 *state)
{
    EmberNetworkStatus st = emberAfNetworkState();
    *state = st;
    log_d("%s : 0x%02X\n", __func__, st);
    return RET_SUCCESS;
}

int apro_handle_get_net_param(void)
{
    int ret_val = RET_ERROR;
    EmberNodeType nodeType;
    EmberNetworkParameters params;
    EmberStatus status = emberAfGetNetworkParameters(&nodeType, &params);
    if(status == EMBER_SUCCESS)
    {
        int i;
        log_i("net_param: Node type 0x%X\n", nodeType);
        log_i("net_param: Extended PAN ID 0x");
        for (i = 0; i < 8; i++)
        {
            debug_printf("%X", params.extendedPanId[i]);
        }
        debug_printf("\n");
        log_i("net_param: PAN ID 0x%2X\n", params.panId);
        log_i("net_param: Radio TX power %d\n", params.radioTxPower);
        log_i("net_param: Radio channel %d\n", params.radioChannel);
        log_i("net_param: Join method 0x%X\n", params.joinMethod);
        log_i("net_param: Network manager ID 0x%2X\n", params.nwkManagerId);
        log_i("net_param: Network update ID 0x%X\n", params.nwkUpdateId);
        log_i("net_param: Channel mask 0x%4X\n", params.channels);
        ret_val = RET_SUCCESS;
    }
    else
    {
        log_i("net_param: Get network params failed with error 0x%X\n", status);
    }

    return ret_val;
}

int apro_handle_get_node_type(void)
{
    int ret_val = RET_ERROR;
    EmberNodeType nodeType;
    EzspStatus st = emberAfGetNodeType(&nodeType);
    if(st == EMBER_SUCCESS)
    {
        log_d("%s %u\n", __func__, nodeType);
        ret_val = RET_SUCCESS;
    }
    else
    {
        log_i("%s error[0x%02x]\n", __func__, st);
    }
    return ret_val;
}

int apro_handle_get_ncp_config(u8 id)
{
    log_d("%s\n", __func__);
    //EzspConfigId configId = (uint8_t)id;
    //uint8_t val = emberAfGetNcpConfigItem(configId);
    //log_d("%s: id[0x%02x], val[0x%02x]\n", __func__, configId, val);
    return RET_SUCCESS;
}

int apro_handle_get_route_table(dest_node_t *data)
{
    uint8_t i;
    memset((char*)data, 0, sizeof(dest_node_t));
    for (i = 0; i < emberAfGetSourceRouteTableFilledSize(); i++)
    {
        uint8_t index = i;
        uint8_t closerIndex;
        EmberNodeId destination;
        EmberStatus status = emberAfGetSourceRouteTableEntry(index,
                                                         &destination,
                                                         &closerIndex);
        if (status != EMBER_SUCCESS)
        {
            log_i("0 of 0 total entries.\n");
            return RET_ERROR;
        }

        debug_printf("%d: 0x%2X -> ", index, destination);
        data->node_id[data->count++] = (u16)destination;

        while (closerIndex != 0xFF)
        {
            index = closerIndex;
            status = emberAfGetSourceRouteTableEntry(index,
                                               &destination,
                                               &closerIndex);
            if (status == EMBER_SUCCESS)
            {
                debug_printf("0x%2X -> ", destination);
            }
            else
            {
                debug_printf("NULL ");
                return RET_ERROR;
            }
        }
        debug_printf("0x%2X (Me)\n", emberAfGetNodeId());
    }
    log_i("%d of %d total entries.\n",
                    emberAfGetSourceRouteTableFilledSize(),
                    emberAfGetSourceRouteTableTotalSize());
    return RET_SUCCESS;
}

int apro_handle_get_route_size(void)
{
    log_d("%d of %d total entries.\n",
                      emberAfGetSourceRouteTableFilledSize(),
                      emberAfGetSourceRouteTableTotalSize());
    return RET_SUCCESS;
}

int apro_handle_get_child(child_data_t *data)
{
    uint8_t size = emberAfGetChildTableSize();
    uint8_t i;
    const char * types[] = {
        "Unknown",
        "Coordin",
        "Router ",
        "RxOn   ",
        "Sleepy ",
        "???    ",
    };
    uint8_t used = 0;
    uint32_t returnFrameCounter = 0;

    memset((char*)data, 0, sizeof(child_data_t));
    log_i("\n");
    debug_printf("#  type    id     eui64               timeout         framecounter  power  phy\n");
    for (i = 0; i < size; i++)
    {
        EmberChildData childData;
        EmberStatus status = emberAfGetChildData(i, &childData);
        if (status != EMBER_SUCCESS)
        {
            continue;
        }
        if (childData.type > EMBER_SLEEPY_END_DEVICE)
        {
            childData.type = EMBER_SLEEPY_END_DEVICE + 1;
        }
        used++;
        debug_printf("%d: %s 0x%2X ", i, types[childData.type], childData.id);
        emberAfAppDebugExec(emberAfPrintBigEndianEui64(childData.eui64));
        debug_printf(" ");
        if (childData.timeout == SECONDS_10)
        {
            debug_printf("10 sec");
        }
        else
        {
            debug_printf("%d min", (2 << (childData.timeout - 1)));
        }

        //Add in the Frame Counter information
        status = emberGetNeighborFrameCounter(childData.eui64, &returnFrameCounter);
        if (status != EMBER_SUCCESS)
        {
            returnFrameCounter = 0xFFFFFFFF;
        }

        debug_printf(" 0x%4X  0x%02X  0x%02X \n",
                    returnFrameCounter, childData.power, childData.phy);

        if(used >= APRO_MAX_NODE)
            continue;
        memcpy((char*)data->child[data->count].eui64, (char*)childData.eui64, 8);
        data->child[data->count].type = childData.type;
        data->child[data->count].id = childData.id;
        data->child[data->count].timeout = childData.timeout;
        data->child[data->count].power = childData.power;
        data->child[data->count].phy = childData.phy;
        data->count = used;
    }
    log_i("%d of %d entries used.\n", used, size);
    return RET_SUCCESS;
}

int apro_handle_get_neighbor(nei_table_t *data)
{
    uint8_t used = 0;
    uint8_t i;
    EmberNeighborTableEntry n;
    uint32_t returnFrameCounter = 0;

    memset((char*)data, 0, sizeof(nei_table_t));

    log_i("\n#  id     lqi  in  out  age  "
                    #ifdef RECORD_LQI_RSSI
                    "lastRssi lastLqi "
                    #endif // RECORD_LQI_RSSI
                    "framecounter eui\n");
    for (i = 0; i < emberAfGetNeighborTableSize(); i++)
    {
        EmberStatus status = emberGetNeighbor(i, &n);
        if((status != EMBER_SUCCESS) || (n.shortId == EMBER_NULL_NODE_ID))
        {
            continue;
        }
        used++;

        //Add in Frame Counter information
        status = emberGetNeighborFrameCounter(n.longId, &returnFrameCounter);
        if (status != EMBER_SUCCESS)
        {
            returnFrameCounter = 0xFFFFFFFF;
        }

        debug_printf("%d: 0x%2X %d  %d   %d    %d    "
                    #ifdef RECORD_LQI_RSSI
                    "%d      %d     "
                    #endif // RECORD_LQI_RSSI
                    "0x%4X  "
                    ,
                    i,
                    n.shortId,
                    n.averageLqi,
                    n.inCost,
                    n.outCost,
                    n.age,
                    #ifdef RECORD_LQI_RSSI
                    neighborRssiTable[i],
                    neighborLqiTable[i],
                    #endif // RECORD_LQI_RSSI
                    returnFrameCounter
                    );
        emberAfAppDebugExec(emberAfPrintBigEndianEui64(n.longId));
        debug_printf("\n");
        emberAfAppFlush();

        if(used >= APRO_MAX_NODE)
            continue;
        data->neighbor[data->count].net_id = n.shortId;
        data->neighbor[data->count].average_lqi = n.averageLqi;
        data->neighbor[data->count].in_cost = n.inCost;
        data->neighbor[data->count].out_cost = n.outCost;
        data->neighbor[data->count].age = n.age;
        memcpy((char*)data->neighbor[data->count].eui64, (char*)n.longId, 8);
        data->count = used;
    }
    log_i("%d of %d entries used.\n", used, emberAfGetNeighborTableSize());
  return RET_SUCCESS;
}

int apro_handle_get_node_info(void)
{
    EmberNodeId nodeThatSentLeave;
    EmberRejoinReason rejoinReason = emberGetLastRejoinReason();
    EmberLeaveReason  leaveReason =  emberGetLastLeaveReason(&nodeThatSentLeave);
    EmberNodeId id = emberAfGetNodeId();
    EmberNodeType type = EMBER_UNKNOWN_DEVICE;
    EmberNetworkParameters parameters;

    if (!emAfProIsCurrentNetwork())
    {
        return RET_ERROR;
    }

    emberAfGetNetworkParameters(&type, &parameters);

  log_i("Stack Profile: %d\n", emberAfGetStackProfile());
  log_i("Channel:       %d\n", parameters.radioChannel);
  log_i("Node ID:       0x%2x\n", id);
  log_i("PAN ID:        0x%2X\n", parameters.panId);
  debug_printf("Extended PAN:  ");
  emberAfPrintBigEndianEui64(parameters.extendedPanId);
  debug_printf("\nNWK Update ID: 0x%X\n", parameters.nwkUpdateId);

  debug_printf("NWK Manager ID: 0x%2X\n", parameters.nwkManagerId);
  debug_printf("NWK Manager Channels: ");
  emberAfPrintChannelListFromMask(parameters.channels);
  debug_printf("\nSend Multicasts to sleepies: %p\n",
                    (emberAfGetSleepyMulticastConfig()
                     ? "yes"
                     : "no"));

  debug_printf("Node that sent leave: 0x%2X\n", nodeThatSentLeave);
  log_i("Last Rejoin reason[0x%02x] Last leave reason[0x%02x]\n", rejoinReason, leaveReason);
  return RET_SUCCESS;
}

int apro_handle_get_node_eui64(u16 node, u8* eui64)
{
    int ret_val = RET_ERROR;
    EmberStatus st = emberLookupEui64ByNodeId(node, eui64);
    if(st == EMBER_SUCCESS)
    {
        log_d("%s node[0x%04X]\n", __func__, node);
        emberAfPrintBigEndianEui64(eui64);
        debug_printf("\n");
        ret_val = RET_SUCCESS;
    }
    else
    {
        log_i("%s error[0x%02x]\n", __func__, st);
    }
    return ret_val;
}

int apro_handle_get_device_ep(u16 id)
{
    int ret_val = RET_ERROR;
    EmberStatus st = emberActiveEndpointsRequest((EmberNodeId)id, EMBER_APS_OPTION_RETRY);
    if(st == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_d("Active EP request status: 0x%X\n", st);
    return ret_val;
}

int apro_handle_get_device_cluster_id(u16 id, u8 ep)
{
    int ret_val = RET_ERROR;
    EmberStatus status = emberSimpleDescriptorRequest((EmberNodeId)id,
                                                      (uint8_t)ep,
                                                      EMBER_AF_DEFAULT_APS_OPTIONS);
    if(status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_d("ZDO simple desc req %x\n", status);
    return ret_val;
}

int apro_handle_bind_req(u8 endpoint)
{
    int ret_val = RET_ERROR;
    EmberStatus st = emberAfSendEndDeviceBind(endpoint);
    if(st == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_d("%s[%d]: 0x%02x\n", __func__, st, endpoint);
    return ret_val;
}

int apro_handle_bind_clear(void)
{
    int ret_val = RET_ERROR;
    EmberStatus st = emberClearBindingTable();
    if(st == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_d("%s status[0x%02x]\n", __func__, st);
    return ret_val;
}

int apro_handle_bind_print(void)
{
    uint8_t i;
    EmberBindingTableEntry result;

    const char * typeStrings[] = {
        "EMPTY",
        "UNICA",
        "M2ONE",
        "MULTI",
        "?    ",
    };
    uint8_t bindings = 0;

    log_d("#  type   nwk  loc   rem   clus   node   eui\n");
    for (i = 0; i < emberAfGetBindingTableSize(); i++)
    {
        EmberStatus status = emberGetBinding(i, &result);
        if (status == EMBER_SUCCESS)
        {
            if (result.type > EMBER_MULTICAST_BINDING)
            {
                result.type = 4;  // last entry in the string list above
            }
            if (result.type != EMBER_UNUSED_BINDING)
            {
                bindings++;
                log_d("%d: %s  %d    0x%x  0x%x  0x%2x 0x%2x %X%X%X%X%X%X%X%X\n",
                        i,
                        typeStrings[result.type],
                        result.networkIndex,
                        result.local,
                        result.remote,
                        result.clusterId,
                        emberGetBindingRemoteNodeId(i),
                        result.identifier[7],
                        result.identifier[6],
                        result.identifier[5],
                        result.identifier[4],
                        result.identifier[3],
                        result.identifier[2],
                        result.identifier[1],
                        result.identifier[0]);
                //emberAfAppDebugExec(emberAfPrintBigEndianEui64(result.identifier));
            }
        } 
        else
        {
            log_d("0x%x: emberGetBinding Error: %x\n", status);
        }
    }
    log_d("%d of %d bindings used\n", bindings, emberAfGetBindingTableSize());
    return RET_SUCCESS;
}

int apro_handle_net_form(u8 security, u16 pan_id, s8 power, u8 ch)
{
    int ret_val = RET_ERROR;
    EmberStatus status = emberAfPluginNetworkCreatorNetworkForm((bool)security,
                                                                (EmberPanId)pan_id,
                                                                (int8_t)power,
                                                                (uint8_t)ch);
    if(status == EMBER_SUCCESS)
    {
        log_i("%s [0x%x] panID:0x%x TxPower:0x%x Channel:0x%x \n",
                                __func__, status, pan_id, power, ch);
        ret_val = RET_SUCCESS;
    }
    else
    {
        log_i("%s error[0x%02x]\n", __func__, status);
    }
    return ret_val;
}

int apro_handle_net_start(void)
{
    int ret_val = RET_ERROR;
    bool centralizedSecurity = 1;
    EmberStatus status = emberAfPluginNetworkCreatorStart(centralizedSecurity);
    if(status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s [0x%x]\n",__func__, status);
    return ret_val;
}

int apro_handle_net_stop(void)
{
    emberAfPluginNetworkCreatorStop();
    log_i("%s\n",__func__);
    return RET_SUCCESS;
}

// network joining open/close
int apro_handle_net_open(void)
{
    int ret_val = RET_ERROR;
    EmberStatus status = emberAfPluginNetworkCreatorSecurityOpenNetwork();
    if (status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s [0x%x]\n",__func__, status);
    return ret_val;
}

int apro_handle_net_close(void)
{
    int ret_val = RET_ERROR;
    EmberStatus status = emberAfPluginNetworkCreatorSecurityCloseNetwork();
    if(status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s [0x%x]\n",__func__, status);
    return ret_val;
}

int apro_handle_net_leave(void)
{
    int ret_val = RET_ERROR;
    EmberStatus status = emberLeaveNetwork();
    if (status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s [0x%x]\n",__func__, status);
    return ret_val;
}

int apro_handle_remove_device(u16 net_id)
{
    int ret_val = RET_ERROR;
    EmberEUI64 nullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t options = 0; // EMBER_ZIGBEE_LEAVE_AND_REJOIN;
    EmberStatus status = EMBER_SUCCESS;
    status = emberLeaveRequest((EmberNodeId)net_id,
                               nullEui64,
                               options,
                               EMBER_APS_OPTION_RETRY);
    //status = emberSendRemoveDevice(parent_id, parent_eui64, target_eui64);
    if (status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s [0x%x]\n", __func__, status);
    return ret_val;
}

static void apro_cb_send_result(EmberOutgoingMessageType type,
                                           uint16_t indexOrDestination,
                                           EmberApsFrame *apsFrame,
                                           uint16_t msgLen,
                                           uint8_t *message,
                                           EmberStatus status)
{
#if 0
    log_d("EmberStatus:0x%02x\n", status);
    log_d("OutMsgType:0x%02x\n", type);
    log_d("DestId:0x%04x\n", indexOrDestination);
    log_d("ProfileID:0x%04x\n", apsFrame->profileId);
    log_d("ClusterID:0x%04x\n", apsFrame->clusterId);
    log_d("Src EndPoint:0x%02x\n", apsFrame->sourceEndpoint);
    log_d("Dest EndPoint:0x%02x\n", apsFrame->destinationEndpoint);
    log_d("Options:0x%04x\n", apsFrame->options);
    log_d("GroupId:0x%02x\n", apsFrame->groupId);
    log_d("Sequence:0x%02x\n", apsFrame->sequence);
    log_d("Radius:0x%02x\n", apsFrame->radius);

    int i = 0;
    log_d("Data: ");
    for(i = 0; i < msgLen; i++)
    {
        debug_printf("%02x ", message[i]);
    }
    debug_printf("\n");
#endif

    cb_pre_cmd_t data = {0,};
    data.net_id = indexOrDestination;
    data.profile_id = apsFrame->profileId;
    data.cluster_id = apsFrame->clusterId;
    data.src_ep = apsFrame->sourceEndpoint;
    data.dest_ep = apsFrame->destinationEndpoint;
    data.options = apsFrame->options;
    data.group_id = apsFrame->groupId;
    data.sequence = apsFrame->sequence;
    data.radius = apsFrame->radius;
    data.msg_type = type;
    data.status = status;
    data.buf_len = msgLen;
    memcpy((char*)&data.buf, (char*)message, msgLen);

    put_event(EV_CB_SEND_RESP, EV_CMD_NONE, (char*)&data, sizeof(data));
}

int apro_handle_send_unicast(zb_frame_t frame)
{
    log_d("%s\n", __func__);
    log_d("dest_id 0x%04X\n", frame.dest_id);
    log_d("profile_id 0x%04X\n", frame.profile_id);
    log_d("cluster_id 0x%04X\n", frame.cluster_id);
    log_d("src_ep 0x%02X\n", frame.src_ep);
    log_d("dest_ep 0x%02X\n", frame.dest_ep);
    log_d("sequence 0x%02X\n", frame.sequence);

    int ret_val = RET_ERROR;
    EmberOutgoingMessageType type = EMBER_OUTGOING_DIRECT;
    uint16_t indexOrDestination = (uint16_t)frame.dest_id;
    EmberApsFrame apsFrame;
    uint8_t messageContents[EZSP_MAX_FRAME_LENGTH] = {0,};
    uint8_t length = 0;

    apsFrame.profileId = frame.profile_id;
    apsFrame.clusterId = frame.cluster_id;
    apsFrame.sourceEndpoint = frame.src_ep;
    apsFrame.destinationEndpoint = frame.dest_ep;
    if(frame.option == 0)
        apsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
    else
        apsFrame.options = frame.option;
    apsFrame.groupId = frame.group_id;
    apsFrame.sequence = frame.sequence;
    if(frame.radius == 0)
        apsFrame.radius = ZA_MAX_HOPS;
    else
        apsFrame.radius = frame.radius;

    length = frame.msg_len;
    memcpy((char*)messageContents, (char*)frame.msg, length);

    EmberStatus status = emberAfSendUnicastWithCallback(type,
                                indexOrDestination,
                                &apsFrame,
                                length,
                                (uint8_t*)&messageContents,
                                apro_cb_send_result);

    if (status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s status[0x%02x]\n", __func__, status);
    return ret_val;
}


int apro_handle_send_broadcast(zb_frame_t frame)
{
    log_d("%s\n", __func__);
    log_d("profile_id 0x%04X\n", frame.profile_id);
    log_d("cluster_id 0x%04X\n", frame.cluster_id);
    log_d("src_ep 0x%02X\n", frame.src_ep);
    log_d("dest_ep 0x%02X\n", frame.dest_ep);

    int ret_val = RET_ERROR;
    EmberNodeId destination = EMBER_SLEEPY_BROADCAST_ADDRESS;
    EmberApsFrame apsFrame;
    uint8_t message[EZSP_MAX_FRAME_LENGTH] = {0,};
    uint16_t messageLength = 0;

    apsFrame.profileId = frame.profile_id;
    apsFrame.clusterId = frame.cluster_id;
    apsFrame.sourceEndpoint = frame.src_ep;
    apsFrame.destinationEndpoint = frame.dest_ep;
    if(frame.option == 0)
        apsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
    else
        apsFrame.options = frame.option;
    apsFrame.groupId = frame.group_id;
    apsFrame.sequence = frame.sequence;
    if(frame.radius == 0)
        apsFrame.radius = ZA_MAX_HOPS;
    else
        apsFrame.radius = frame.radius;

    messageLength = frame.msg_len;
    memcpy((char*)message, (char*)frame.msg, messageLength);

    EmberStatus status = emberAfSendBroadcastWithCallback(destination,
                                            &apsFrame,
                                            messageLength,
                                            message,
                                            apro_cb_send_result);
    if (status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s status[0x%02x]\n", __func__, status);
    return ret_val;
}

int apro_handle_send_multicast(void)
{
    int ret_val = RET_ERROR;
    EmberNodeId destination = EMBER_OUTGOING_MULTICAST; // group id
    EmberApsFrame apsFrame;
    uint8_t sequence = emberNextZigDevRequestSequence();
    uint8_t message[] = { 1, sequence, ZCL_TOGGLE_COMMAND_ID};
    uint16_t messageLength = sizeof(message);

    apsFrame.profileId = HA_PROFILE_ID;
    apsFrame.clusterId = ZCL_BASIC_CLUSTER_ID;
    apsFrame.sourceEndpoint = 1;
    apsFrame.destinationEndpoint = 1;
    apsFrame.options = EMBER_AF_DEFAULT_APS_OPTIONS;
    apsFrame.groupId = 0x0;
    apsFrame.sequence = sequence;
    apsFrame.radius = ZA_MAX_HOPS;

    EmberStatus status = emberAfSendMulticastWithCallback(destination,
                                            &apsFrame,
                                            messageLength,
                                            message,
                                            apro_cb_send_result);
    if (status == EMBER_SUCCESS)
    {
        ret_val = RET_SUCCESS;
    }
    log_i("%s status[0x%02x]\n", __func__, status);
    return ret_val;
}

int apro_handle_ncp_reboot(void)
{
    log_i("%s\n", __func__);
    ezspErrorHandler(EZSP_NO_ERROR); //emAfResetAndInitNCP();
    return RET_SUCCESS;
}

int apro_handle_exit(void)
{
    log_i("%s\n", __func__);
    halReboot();
    while(1)
    {
        usleep(100 * 1000);
    }
    return RET_SUCCESS;
}

/*
int apro_handle_factory_reset(void)
{
    EzspStatus status = emberSecureEzspResetToFactoryDefaults();
    if(status == EZSP_SUCCESS)
    {
      log_i("Reset Security");
    }
    else
    {
      log_i("Failed to Reset Security, status:0x%x", status);
    }
    return RET_SUCCESS;
}
*/

void apro_cb_init(void)
{
    apro_init();
}

void apro_cb_proc(void)
{
    apro_proc();
}

void apro_cb_reset_attribute(uint8_t endpointId)
{
    log_d("%s endpoint[0x%02X]\n", __func__, endpointId);
    put_event(EV_CB_RST_ATTR, EV_CMD_NONE, (char*)&endpointId, sizeof(uint8_t));
}

bool apro_cb_pre_cmd_received(EmberAfClusterCommand* cmd)
{
    log_d("%s\n", __func__);
#if 0
    log_d("NodeId:0x%04x\n", cmd->source);

    log_d("ProfileID:0x%04x\n", cmd->apsFrame->profileId);
    log_d("ClusterID:0x%04x\n", cmd->apsFrame->clusterId);
    log_d("Src EndPoint:0x%02x\n", cmd->apsFrame->sourceEndpoint);
    log_d("Dest EndPoint:0x%02x\n", cmd->apsFrame->destinationEndpoint);
    log_d("Options:0x%04x\n", cmd->apsFrame->options);
    log_d("GroupId:0x%02x\n", cmd->apsFrame->groupId);
    log_d("Sequence:0x%02x\n", cmd->apsFrame->sequence);
    log_d("Radius:0x%02x\n", cmd->apsFrame->radius);

    int i = 0;
    log_d("Data: ");
    for(i = 0; i < cmd->bufLen; i++)
    {
        debug_printf("%02x ", cmd->buffer[i]);
    }
    debug_printf("\n");
#endif
    cb_pre_cmd_t data = {0,};
    data.net_id = cmd->source;
    data.profile_id = cmd->apsFrame->profileId;
    data.cluster_id = cmd->apsFrame->clusterId;
    data.src_ep = cmd->apsFrame->sourceEndpoint;
    data.dest_ep = cmd->apsFrame->destinationEndpoint;
    data.options = cmd->apsFrame->options;
    data.group_id = cmd->apsFrame->groupId;
    data.sequence = cmd->apsFrame->sequence;
    data.radius = cmd->apsFrame->radius;
    data.msg_type = cmd->type;
    data.cmd_id = cmd->commandId;
    data.buf_len = cmd->bufLen;
    memcpy((char*)&data.buf, (char*)cmd->buffer, cmd->bufLen);

    put_event(EV_CB_PRE_CMD, EV_CMD_NONE, (char*)&data, sizeof(data));
    return false;
}

bool apro_cb_pre_zdo_msg_received(EmberNodeId emberNodeId, EmberApsFrame* apsFrame, uint8_t* message, uint16_t length)
{
    log_d("%s\n", __func__);

#if 0
    log_d("NodeId:0x%04x\n", emberNodeId);
    log_d("ProfileID:0x%04x\n", apsFrame->profileId);
    log_d("ClusterID:0x%04x\n", apsFrame->clusterId);
    log_d("Src EndPoint:0x%02x\n", apsFrame->sourceEndpoint);
    log_d("Dest EndPoint:0x%02x\n", apsFrame->destinationEndpoint);
    log_d("Options:0x%04x\n", apsFrame->options);
    log_d("GroupId:0x%02x\n", apsFrame->groupId);
    log_d("Sequence:0x%02x\n", apsFrame->sequence);
    log_d("Radius:0x%02x\n", apsFrame->radius);

    int i = 0;
    log_d("Data: ");
    for(i = 0; i < length; i++)
    {
        debug_printf("%02x ", message[i]);
    }
    debug_printf("\n");
#endif

    cb_pre_cmd_t data = {0,};
    data.net_id = emberNodeId;
    data.profile_id = apsFrame->profileId;
    data.cluster_id = apsFrame->clusterId;
    data.src_ep = apsFrame->sourceEndpoint;
    data.dest_ep = apsFrame->destinationEndpoint;
    data.options = apsFrame->options;
    data.group_id = apsFrame->groupId;
    data.sequence = apsFrame->sequence;
    data.radius = apsFrame->radius;
    data.buf_len = length;
    memcpy((char*)&data.buf, (char*)message, length);

    put_event(EV_CB_ZDO_MSG, EV_CMD_NONE, (char*)&data, sizeof(data));

  return false;
}

bool apro_cb_retrieve_attr_and_craft_response(uint8_t endpoint, EmberAfClusterId clusterId, EmberAfAttributeId attrId, uint8_t mask, uint16_t maunfacturerCode, uint16_t readLength)
{
    log_d("%s\n", __func__);
    //log_d("Cluster ID: 0x%04x, AttrID: 0x%04x EndPoint: 0x%02x\n", clusterId, attrId, endpoint);

    cb_reteriv_t reterive;
    reterive.endpoint = endpoint;
    reterive.cluster_id = clusterId;
    reterive.attr_id = attrId;
    reterive.mask = mask;
    reterive.maunfacturer_code = maunfacturerCode;
    reterive.read_len = readLength;
    put_event(EV_CB_RETRIVE_ATTR, EV_CMD_NONE, (char*)&reterive, sizeof(reterive));
    return false;
}

void apro_cb_zigbee_key_establishment(EmberEUI64 partner, EmberKeyStatus status)
{
    log_d("%s\n", __func__);
    cb_key_est_t key;
    key.status = status;
    memcpy((char*)&key.eui64, (char*)&partner, sizeof(EmberEUI64));
    put_event(EV_CB_KEY_EST, EV_CMD_NONE, (char*)&key, sizeof(key));
}

bool apro_cb_read_attr_response(EmberAfClusterId clusterId, uint8_t *buffer, uint16_t bufLen)
{
    log_d("%s\n", __func__);
#if 0
    log_d("Cluster ID: 0x%04x\n", clusterId);
    int i = 0;
    log_d("RX: ");
    for(i = 0; i < bufLen; i++)
    {
      debug_printf("0x%02x ", buffer[i]);
    }
    debug_printf("\n");
#endif
    cb_attr_t resp = {0,};
    resp.cluster_id = clusterId;
    resp.buf_len = bufLen;
    memcpy((char*)&resp.buf, buffer, bufLen);
    put_event(EV_CB_ATTR_RESP, EV_CMD_NONE, (char*)&resp, sizeof(resp));

    return false;
}

bool apro_cb_report_attribute(EmberAfClusterId clusterId, uint8_t *buffer, uint16_t bufLen)
{
    log_d("%s\n", __func__);
#if 0
    log_d("Cluster ID: 0x%04x\n", clusterId);
    int i = 0;
    log_d("RX: ");
    for(i = 0; i < bufLen; i++)
    {
      debug_printf("0x%02x ", buffer[i]);
    }
    debug_printf("\n");
#endif

    cb_attr_t resp = {0,};
    resp.cluster_id = clusterId;
    resp.buf_len = bufLen;
    memcpy((char*)&resp.buf, buffer, bufLen);
    put_event(EV_CB_REPORT_ATTR, EV_CMD_NONE, (char*)&resp, sizeof(resp));

    return false;
}

bool apro_cb_pre_msg_recv(EmberAfIncomingMessage* im)
{
    log_d("%s net_id[0x%04x] lqi[0x%02x] rssi[%d]\n",
        __func__, im->source, im->lastHopLqi, im->lastHopRssi);

    cb_rssi_t resp;
    resp.net_id = im->source;
    resp.lqi = im->lastHopLqi;
    resp.rssi = im->lastHopRssi;
    put_event(EV_CB_PRE_MSG_RCV, EV_CMD_NONE, (char*)&resp, sizeof(resp));
    return false;
}

