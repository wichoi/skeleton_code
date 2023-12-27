#include "af.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-zcl-basic.h"
#include "apro-zcl-onoff.h"
#include "apro-zcl-switch.h"
#include "apro-zcl-level.h"
#include "apro-zcl-temp.h"
#include "apro-zcl-humid.h"
#include "apro-zcl-iaszone.h"

static void apro_cli_void(void)
{
    log_i("%s\n", __func__);
}

static void apro_cli_version(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_VERSION, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_get_eui64(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_EUI64, EV_CMD_GET, NULL, 0);
}

static void apro_cli_node_id(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_NODE, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_pan_id(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_PAN_ID, EV_CMD_GET, NULL, 0);
}

static const char * const pan_id_arg[] = {
    "u16 pan id",
    NULL
};

static void apro_cli_channel(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_CHANNEL, EV_CMD_NONE, NULL, 0);
}

static const char * const set_conf_arg[] = {
    "u8 EzspConfigId",
    "u16 value",
    "str config name",
    NULL
};

static void apro_cli_set_confing(void)
{
    EzspConfigId configId = (uint8_t)emberUnsignedCommandArgument(0);
    uint16_t value = (uint16_t)emberUnsignedCommandArgument(1);
    log_i("%s: id[0x%X], value[0x%X]\n", __func__, configId, value);
    put_event(EV_CONF_SET, EV_CMD_SET, NULL, 0);
}

static void apro_cli_network_state(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_NET_ST, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_get_net_param(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_NET_PARAM, EV_CMD_GET, NULL, 0);
}

static void apro_cli_get_node_type(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_NODE_TYPE, EV_CMD_GET, NULL, 0);
}

static const char * const ncp_conf_id_arg[] = {
  "u8 NCP Config ID",
  NULL
};

static void apro_cli_get_ncp_config(void)
{
    EzspConfigId configId = (uint8_t)emberUnsignedCommandArgument(0);
    log_i("%s: configID[0x%X]\n", __func__, configId);
    put_event(EV_CONF_NCP, EV_CMD_GET, (char*)&configId, sizeof(EzspConfigId));
}

static void apro_cli_get_route_table(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_ROUTE_TABLE, EV_CMD_GET, NULL, 0);
}

static void apro_cli_get_route_size(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_ROUTE_SIZE, EV_CMD_GET, NULL, 0);
}

static void apro_cli_get_child_table(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_CHILD, EV_CMD_GET, NULL, 0);
}

static void apro_cli_get_neighbor(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_NEIGHBOR, EV_CMD_GET, NULL, 0);
}

static const char * const dest_net_id_arg[] = {
  "u16 Destination Address",
  NULL
};

static void apro_cli_get_device_ep(void)
{
    EmberNodeId target = (EmberNodeId)emberUnsignedCommandArgument(0);
    log_i("%s nodeId[0x%x]\n", __func__, target);
    put_event(EV_CONF_GET_EP, EV_CMD_GET, (char*)&target, sizeof(EmberNodeId));
}

static const char * const get_net_id_arg[] = {
  "u16 Device Address",
  "u8 End Point",
  NULL
};

static void apro_cli_get_device_cluster_id(void)
{
    EmberNodeId target = (EmberNodeId)emberUnsignedCommandArgument(0);
    uint8_t targetEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
    log_i("%s nodeId[0x%x] endPoint[0x%x]\n", __func__, target, targetEndpoint);
    char buf[3];
    memcpy(buf, (char*)&target, 2);
    buf[2] = targetEndpoint;
    put_event(EV_CONF_GET_CL, EV_CMD_GET, buf, sizeof(buf));
}

static void apro_cli_get_node_info(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_NODE_INFO, EV_CMD_GET, NULL, 0);
}

static const char * const bind_req_arg[] = {
    "u8 end point index",
    NULL
};

static void apro_cli_bind_req(void)
{
    uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);

    log_i("%s EndPoint[0x%x]\n", __func__, endpoint);
    put_event(EV_CONF_BIND_REQ, EV_CMD_GET, (char*)&endpoint, sizeof(uint8_t));
}

static void apro_cli_bind_clear(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_BIND_CLEAR, EV_CMD_GET, NULL, 0);
}

static void apro_cli_bind_print(void)
{
    log_i("%s\n", __func__);
    put_event(EV_CONF_BIND_PRINT, EV_CMD_GET, NULL, 0);
}

static const char * const net_form_arg[] = {
  "u8 centralized networ ...",
  "u16 PanID",
  "s8 Tx power of the network to be formed",
  "u8 channel",
  NULL
};

static void apro_cli_net_form(void)
{
    bool centralizedSecurity = (bool)emberUnsignedCommandArgument(0);
    EmberPanId panId = (EmberPanId)emberUnsignedCommandArgument(1);
    int8_t radioTxPower = (int8_t)emberSignedCommandArgument(2);
    uint8_t channel = (uint8_t)emberUnsignedCommandArgument(3);

    log_i("%s sec[%d] panID[0x%x] TxPower[0x%x] Channel[0x%x]\n",
        __func__, centralizedSecurity, panId, radioTxPower, channel);
    char buf[5] = {0,};
    buf[0] = (char)centralizedSecurity;
    memcpy((char*)&buf[1], (char*)&panId, 2);
    buf[3] = radioTxPower;
    buf[4] = channel;

    put_event(EV_NET_NONE, EV_CMD_NONE, buf, 5);
}

static void apro_cli_net_start(void)
{
    log_i("%s\n", __func__);
    put_event(EV_NET_START, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_net_stop(void)
{
    log_i("%s\n", __func__);
    put_event(EV_NET_STOP, EV_CMD_NONE, NULL, 0);
}

// network joining open/close
static void apro_cli_net_open(void)
{
    log_i("%s\n", __func__);
    put_event(EV_NET_OPEN, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_net_close(void)
{
    log_i("%s\n", __func__);
    put_event(EV_NET_CLOSE, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_net_leave(void)
{
    log_i("%s\n", __func__);
    put_event(EV_NET_LEAVE, EV_CMD_NONE, NULL, 0);
}

static const char * const remove_dev_arg[] = {
  "u16 parentShortId",
  NULL
};

static void apro_cli_net_remove_device(void)
{
    u16 net_id = (u16)emberUnsignedCommandArgument(0);
    log_i("%s net_id[0x%04X]\n", __func__, net_id);
    put_event(EV_NET_REMOVE_DEV, EV_CMD_NONE, (char*)&net_id, sizeof(net_id));
}

static const char * const send_zcl_cmd_arg[] = {
  "u16 Destination Address",
  "u8 Command (0 Off, 1 On, 2 Toggle)",
  NULL
};

static void apro_cli_zcl_onoff_cmd(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    u16 cmd = (u16)emberUnsignedCommandArgument(1);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;

    switch(cmd)
    {
    case 0:
        apro_zcl_onoff_cmd_off(dest_id, dest_ep, &frame);
        break;
    case 1:
        apro_zcl_onoff_cmd_on(dest_id, dest_ep, &frame);
        break;
    case 2:
        apro_zcl_onoff_cmd_toggle(dest_id, dest_ep, &frame);
        break;
    default:
        apro_zcl_onoff_cmd_toggle(dest_id, dest_ep, &frame);
        break;
    }

#if 0
    log_i("%s\n", __func__);
    log_i("dest_id 0x%04X\n", frame.dest_id);
    log_i("profile_id 0x%04X\n", frame.profile_id);
    log_i("cluster_id 0x%04X\n", frame.cluster_id);
    log_i("src_ep 0x%02X\n", frame.src_ep);
    log_i("dest_ep 0x%02X\n", frame.dest_ep);
#endif

    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static const char * const send_zcl_attr_arg[] = {
  "u16 Destination Address",
  NULL
};

static void apro_cli_zcl_onoff_attr(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_onoff_attr(dest_id, dest_ep, &frame);

#if 0
    log_i("%s\n", __func__);
    log_i("dest_id 0x%04X\n", frame.dest_id);
    log_i("profile_id 0x%04X\n", frame.profile_id);
    log_i("cluster_id 0x%04X\n", frame.cluster_id);
    log_i("src_ep 0x%02X\n", frame.src_ep);
    log_i("dest_ep 0x%02X\n", frame.dest_ep);
#endif

    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static void apro_cli_zcl_switch_attr(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_switch_attr_type(dest_id, dest_ep, &frame);
    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
    apro_zcl_switch_attr_action(dest_id, dest_ep, &frame);
    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static const char * const zcl_level_cmd_arg[] = {
  "u16 Destination Address",
  "u8 Level",
  "u16 Transition time",
  NULL
};

static void apro_cli_zcl_level_cmd(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    u8 level = (u8)emberUnsignedCommandArgument(1);
    u16 tr_time = (u8)emberUnsignedCommandArgument(2);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_level_cmd_move_level(dest_id, dest_ep, level, tr_time, &frame);
    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static void apro_cli_zcl_level_attr(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_level_attr(dest_id, dest_ep, &frame);
    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static void apro_cli_zcl_temp_attr(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_temp_attr(dest_id, dest_ep, &frame);

    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static void apro_cli_zcl_humid_attr(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_humid_attr(dest_id, dest_ep, &frame);

    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static void apro_cli_zcl_motion_attr(void)
{
    EmberNodeId dest_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    zb_frame_t frame = {0,};
    u16 dest_ep = 1;
    apro_zcl_iaszone_attr(dest_id, dest_ep, &frame);

    put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
}

static void apro_cli_send_broadcast(void)
{
    zb_frame_t data = {0,};
    apro_zcl_basic_attribute(0xFFFF, 1, &data);
#if 0
    log_i("%s\n", __func__);
    log_i("profile_id 0x%04X\n", data.profile_id);
    log_i("cluster_id 0x%04X\n", data.cluster_id);
    log_i("src_ep 0x%02X\n", data.src_ep);
    log_i("dest_ep 0x%02X\n", data.dest_ep);
    log_i("ash_ctrl 0x%02X\n", data.ash_ctrl);
    log_i("cmd 0x%02X\n", data.cmd);
    log_i("attr 0x%04X\n", data.attr);
#endif

    put_event(EV_SEND_BR, EV_CMD_NONE, (char*)&data, sizeof(data));
}

static void apro_cli_ipc_recv(void)
{
    log_i("%s\n", __func__);
    put_event(EV_IPC_RECV, EV_CMD_NONE, "recv_data", sizeof("recv_data"));
}

static void apro_cli_ipc_send(void)
{
    log_i("%s\n", __func__);
    put_event(EV_IPC_SEND, EV_CMD_NONE, "send_data", sizeof("send_data"));
}

static const char * const send_ipc_test_arg[] = {
  "u8 IPC test cmd",
  NULL
};
static void aprp_cli_ipc_test(void)
{
    u8 cmd = (u8)emberUnsignedCommandArgument(0);
    log_i("%s cmd[0x%02x]\n", __func__, cmd);

    u8 data[5][16] = {{0x00, 0x00, 0x00, 0x01, 0, 0x04, 0x02, 0, 3, 5, 0x01, 0x2C, 0x00, 0x00, 0x00, 0x00}, // reg_start
                    {0x00, 0x00, 0x00, 0x01, 0, 0x04, 0x02, 0, 3, 5, 0x01, 0x2D, 0x00, 0x00, 0x00, 0x00}, // reg_end
                    {0x00, 0x00, 0x00, 0x01, 0, 0x04, 0x02, 0, 3, 5, 0x01, 0x2E, 0x00, 0x00, 0x00, 0x00}, // reg_done
                    {0x00, 0x00, 0x00, 0x01, 0, 0x04, 0x02, 0, 3, 5, 0x01, 0x2F, 0x00, 0x02, 0x12, 0x34}, // reg_del
                    {0x00, 0x00, 0x00, 0x01, 0, 0x04, 0x02, 0, 3, 5, 0x01, 0x30, 0x00, 0x00, 0x00, 0x00}}; // dev_list

    put_event(EV_IPC_SEND, EV_CMD_NONE, (char*)data[cmd], sizeof(data[cmd]));
}

static const char * const send_add_node_arg[] = {
  "u16 Destination Address",
  NULL
};

static void apro_cli_test_add_node(void)
{
    EmberNodeId node_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    log_i("%s node_id[0x%04x]\n", __func__, node_id);
    put_event(EV_TEST_ADD_NODE, EV_CMD_NONE, (char*)&node_id, sizeof(node_id));
}

static void apro_cli_test_del_node(void)
{
    EmberNodeId node_id = (EmberNodeId)emberUnsignedCommandArgument(0);
    log_i("%s node_id[0x%04x]\n", __func__, node_id);
    put_event(EV_TEST_DEL_NODE, EV_CMD_NONE, (char*)&node_id, sizeof(node_id));
}

static const char * const aging_cmd_arg[] = {
    "u16 test count(1 ~ 65535 times)",
    "u16 test interval(1 ~ 65535 sec)",
    NULL
};

static void apro_cli_aging_start(void)
{
    u16 count = (u16)emberUnsignedCommandArgument(0);
    u16 interval = (u16)emberUnsignedCommandArgument(1);
    log_i("%s count[%u] interval[%u] \n", __func__, count, interval);

    char buf[4];
    memcpy(buf, (char*)&count, 2);
    memcpy((char*)&buf[2], (char*)&interval, 2);
    put_event(EV_TEST_AGING_START, EV_CMD_NONE, buf, sizeof(buf));
}

static void apro_cli_aging_stop(void)
{
    log_i("%s\n", __func__);
    put_event(EV_TEST_AGING_STOP, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_ncp_reboot(void)
{
    log_i("%s\n", __func__);
    put_event(EV_SYS_REBOOT, EV_CMD_NONE, NULL, 0);
}

static void apro_cli_exit(void)
{
    log_i("%s\n", __func__);
    put_event(EV_SYS_EXIT, EV_CMD_NONE, "send_data", sizeof("send_data"));
}

EmberCommandEntry emberAfAproCli[] = {
    emberCommandEntryAction("===============", apro_cli_void, "", "Configuration - ==============="),
    emberCommandEntryAction("ver", apro_cli_version, "", "NCP stack Version"),
    emberCommandEntryAction("eui64", apro_cli_get_eui64, "", "EUI 64-bit ID (an IEEE address)"),
    emberCommandEntryAction("node-id", apro_cli_node_id, "", "Get Node ID"),
    emberCommandEntryAction("pan-id", apro_cli_pan_id, "", "Get Pan ID"),
    emberCommandEntryAction("channel", apro_cli_channel, "", "Get Channel"),
    emberCommandEntryActionWithDetails("set-conf", apro_cli_set_confing, "uvb", "Set End Point Binding", set_conf_arg),
    emberCommandEntryAction("net-st", apro_cli_network_state, "", "Get Netwrok State"),
    emberCommandEntryAction("net-param", apro_cli_get_net_param, "", "Get Network Paramaters"),
    emberCommandEntryAction("node-type", apro_cli_get_node_type, "", "Get Node Type"),
    emberCommandEntryActionWithDetails("ncp-conf", apro_cli_get_ncp_config, "u", "Get NCP Configure", ncp_conf_id_arg),
    emberCommandEntryAction("get-route", apro_cli_get_route_table, "", "Get Route Table"),
    emberCommandEntryAction("get-route-size", apro_cli_get_route_size, "", "Get Route Size"),
    emberCommandEntryAction("get-child", apro_cli_get_child_table, "", "Get Child Node Table"),
    emberCommandEntryAction("get-nei", apro_cli_get_neighbor, "", "Get Neighbor Table"),
    emberCommandEntryActionWithDetails("get-ep", apro_cli_get_device_ep, "v", "Get EndPoint Count of EndDevice", dest_net_id_arg),
    emberCommandEntryActionWithDetails("get-cl", apro_cli_get_device_cluster_id, "vu", "Get Cluster ID of EndDevice", get_net_id_arg),
    emberCommandEntryAction("node-info", apro_cli_get_node_info, "", "Get Node Info"),

    emberCommandEntryAction("===============", apro_cli_void, "", "Bind - ==============="),
    emberCommandEntryActionWithDetails("bind", apro_cli_bind_req, "u", "Set End Point Binding", bind_req_arg),
    emberCommandEntryAction("bind-clear", apro_cli_bind_clear, "", "Clear Binding Table"),
    emberCommandEntryAction("bind-pinrt", apro_cli_bind_print, "", "Print Binding Table"),

    emberCommandEntryAction("===============", apro_cli_void, "", "Zigbee Network - ==============="),
    emberCommandEntryActionWithDetails("net-form", apro_cli_net_form, "uvsu", "Form a network with specified parameters.", net_form_arg),
    emberCommandEntryAction("net-start", apro_cli_net_start, "", "Network Formation Process Start"),
    emberCommandEntryAction("net-stop", apro_cli_net_stop, "", "Network Formation Process Stop"),
    emberCommandEntryAction("net-open", apro_cli_net_open, "", "Open the network for joining"),
    emberCommandEntryAction("net-close", apro_cli_net_close, "", "Close the network for joining"),
    emberCommandEntryAction("net-leave", apro_cli_net_leave, "", "Leave the current network"),
    emberCommandEntryActionWithDetails("net-del", apro_cli_net_remove_device, "v", "Reomve Device", remove_dev_arg),

    emberCommandEntryAction("===============", apro_cli_void, "", "Cluster Message Send - ==============="),
    emberCommandEntryActionWithDetails("onoff-cmd", apro_cli_zcl_onoff_cmd, "vu", "OnOff Command", send_zcl_cmd_arg),
    emberCommandEntryActionWithDetails("onoff-attr", apro_cli_zcl_onoff_attr, "v", "OnOff Attribute", send_zcl_attr_arg),

    emberCommandEntryActionWithDetails("switch-attr", apro_cli_zcl_switch_attr, "v", "Switch Attribute", send_zcl_attr_arg),

    emberCommandEntryActionWithDetails("level-cmd", apro_cli_zcl_level_cmd, "vuv", "Level Control Command", zcl_level_cmd_arg),
    emberCommandEntryActionWithDetails("level-attr", apro_cli_zcl_level_attr, "v", "Level Control Attribute", send_zcl_attr_arg),

    emberCommandEntryActionWithDetails("temp-attr", apro_cli_zcl_temp_attr, "v", "Temperature Sensor Attribute", send_zcl_attr_arg),
    emberCommandEntryActionWithDetails("humid-attr", apro_cli_zcl_humid_attr, "v", "Humidity Sensor Attribute", send_zcl_attr_arg),
    emberCommandEntryActionWithDetails("motion-attr", apro_cli_zcl_motion_attr, "v", "Motion Sensor Attribute", send_zcl_attr_arg),

    emberCommandEntryAction("send-br", apro_cli_send_broadcast, "", "Send Broadcast Message (Basic, Version, Read Attr)"),

    emberCommandEntryAction("===============", apro_cli_void, "", "OCF IPC - ==============="),
    emberCommandEntryAction("ipc-recv", apro_cli_ipc_recv, "", "IPC Recv Message"),
    emberCommandEntryAction("ipc-send", apro_cli_ipc_send, "", "IPC Send Message"),
    emberCommandEntryActionWithDetails("ipc-test", aprp_cli_ipc_test, "u", "IPC Test(0:reg start, 1:reg end, 2:reg done, 3:reg del, 4:dev list)", send_ipc_test_arg),

    emberCommandEntryAction("===============", apro_cli_void, "", "Test - ==============="),
    emberCommandEntryActionWithDetails("test-add", apro_cli_test_add_node, "v", "Add Node", send_add_node_arg),
    emberCommandEntryActionWithDetails("test-del", apro_cli_test_del_node, "v", "Del Node", send_add_node_arg),

    emberCommandEntryAction("===============", apro_cli_void, "", "Aging Test - ==============="),
    emberCommandEntryActionWithDetails("aging-start", apro_cli_aging_start, "vv", "Aging Test Start", aging_cmd_arg),
    emberCommandEntryAction("aging-stop", apro_cli_aging_stop, "", "Aging Test Stop"),

    emberCommandEntryAction("===============", apro_cli_void, "", "System Command - ==============="),
    emberCommandEntryAction("ncp-reboot", apro_cli_ncp_reboot, "", "NCP Reboot"),
    emberCommandEntryAction("exit", apro_cli_exit, "", "Exit Zigbee Host"),

    emberCommandEntryAction("===============", apro_cli_void, "", "========================="),
    emberCommandEntryTerminator()
};

