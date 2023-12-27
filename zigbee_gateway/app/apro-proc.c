#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "af.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-state.h"
#include "apro-config.h"
#include "apro-handle.h"
#include "apro-zcl-basic.h"
#include "apro-zcl-onoff.h"
#include "apro-zcl-switch.h"
#include "apro-zcl-level.h"
#include "apro-zcl-temp.h"
#include "apro-zcl-humid.h"
#include "apro-zcl-iaszone.h"
#include "apro-node.h"
#include "apro-ipc-payload.h"
#include "apro-ipc.h"
#include "apro-ocf.h"
#include "apro-ocf-parser.h"
#include "apro-test.h"
#include "apro-proc.h"

static int apro_proc_nop(u32 op, char *data, u32 len)
{
    log_d("%s No Operation !!!\n", __func__);
    return RET_SUCCESS;
}

static int apro_proc_version(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);

    u8 major;
    u8 minor;
    u8 patch;
    u16 build;

    apro_handle_version(&major, &minor, &patch, &build);
    log_i("%s stack ver[%d.%d.%d.%d]\n", __func__, major, minor, patch, build);
    if(op == EV_CMD_IPC_GET)
    {
        char buf[32] = {0,};
        apro_ipc_payload_version(buf, 32, major, minor, patch, build);
        put_event(EV_IPC_SEND, EV_CMD_IPC_RESP, buf, sizeof(buf));
    }
    return RET_SUCCESS;
}

static int apro_proc_eui64(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_eui64();
    return RET_SUCCESS;
}

static int apro_proc_pan_id(u32 op, char *data, u32 len)
{
    log_i("%s: op_code[%d]\n", __func__, op);
    switch(op)
    {
    case EV_CMD_GET:
        apro_handle_eui64();
        break;
    case EV_CMD_SET:
        {
            u16 pan_id;
            memcpy((char*)&pan_id, data, sizeof(u16));
            log_i("%s: 0x%x\n", __func__, pan_id);
        }
        break;
    case EV_CMD_RESP:
        break;
    default:
        apro_handle_eui64();
        break;
    }

    return RET_SUCCESS;
}

static int apro_proc_channel(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_channel();
    return RET_SUCCESS;
}

static int apro_proc_node(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_node_id();
    return RET_SUCCESS;
}

static int apro_proc_set_conf(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    u8 id = 0;
    u16 val = 0;
    apro_handle_set_confing(id, val);
    return RET_SUCCESS;
}

static int apro_proc_net_st(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    u8 st; // EmberNetworkStatus
    apro_handle_get_net_st(&st);
    apro_set_net_st(st);
    return RET_SUCCESS;
}

static int apro_proc_net_param(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_get_net_param();
    return RET_SUCCESS;
}

static int apro_proc_node_type(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_get_node_type();
    return RET_SUCCESS;
}

static int apro_proc_ncp_config(u32 op, char *data, u32 len)
{
    u8 conf_id;
    memcpy((char*)&conf_id, data, sizeof(u8));
    log_i("%s: 0x%x\n", __func__, conf_id);
    apro_handle_get_ncp_config(conf_id);
    return RET_SUCCESS;
}

static int apro_proc_route_table(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    dest_node_t get_data;
    apro_handle_get_route_table(&get_data);

    int i = 0;
    u8 eui64[8] = {0,};
    for(i = 0; i < get_data.count; i++)
    {
        //log_i("id 0x%04X\n", get_data.node_id[i);
        memset(eui64, 0, sizeof(eui64));
        apro_handle_get_node_eui64(get_data.node_id[i], eui64);
        apro_node_add(get_data.node_id[i], eui64);
    }

    return RET_SUCCESS;
}

static int apro_proc_route_size(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_get_route_size();
    return RET_SUCCESS;
}

static int apro_proc_child(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    child_data_t get_data = {0,};
    apro_handle_get_child(&get_data);

    int i = 0;
    for(i = 0; i < get_data.count; i++)
    {
        //log_i("id 0x%04X\n", get_data.child[i].id);
        apro_node_add(get_data.child[i].id, get_data.child[i].eui64);
    }

    return RET_SUCCESS;
}

static int apro_proc_neighbor(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    nei_table_t get_data = {0,};
    apro_handle_get_neighbor(&get_data);

    int i = 0;
    for(i = 0; i < get_data.count; i++)
    {
        //log_i("id 0x%04X\n", get_data.neighbor[i].net_id);
        apro_node_add(get_data.neighbor[i].net_id, get_data.neighbor[i].eui64);
    }

    return RET_SUCCESS;
}

static int apro_proc_node_info(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_get_node_info();

    apro_node_print();
    return RET_SUCCESS;
}

static int apro_proc_get_end_point(u32 op, char *data, u32 len)
{
    u16 node_id;
    memcpy((char*)&node_id, data, sizeof(u16));
    log_i("%s: node_id[0x%x]\n", __func__, node_id);
    apro_handle_get_device_ep(node_id);
    return RET_SUCCESS;
}

static int apro_proc_get_clustor(u32 op, char *data, u32 len)
{
    u16 node_id;
    u8 end_point;
    memcpy((char*)&node_id, data, 2);
    end_point = data[2];
    log_i("%s NodeId[0x%x], EndPoint[0x%x]\n", __func__, node_id, end_point);
    apro_handle_get_device_cluster_id(node_id, end_point);
    return RET_SUCCESS;
}

static int apro_proc_bind_request(u32 op, char *data, u32 len)
{
    u8 end_point = data[0];

    log_i("%s end_point[0x%x]\n", __func__, end_point);
    apro_handle_bind_req(end_point);
    return RET_SUCCESS;
}

static int apro_proc_bind_clear(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_bind_clear();
    return RET_SUCCESS;
}

static int apro_proc_bind_print(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_bind_print();
    return RET_SUCCESS;
}


static int apro_proc_net_form(u32 op, char *data, u32 len)
{
    u8 security = 0;
    u16 pan_id = 0;
    s8 power = 0;
    u8 ch = 0;

    security = data[0];
    memcpy((char*)&pan_id, &data[1], 2);
    power = (s8)data[2];
    ch = data[3];

    log_i("%s sec[%u], pan_id[0x%x], power[%d], ch[%u]\n", __func__,
                                        security, pan_id, power, ch);
    apro_handle_net_form(security, pan_id, power, ch);
    return RET_SUCCESS;
}

static int apro_proc_net_start(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_net_start();
    return RET_SUCCESS;
}

static int apro_proc_net_stop(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_net_stop();
    return RET_SUCCESS;
}

static int apro_proc_net_open(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_net_open();
    return RET_SUCCESS;
}

static int apro_proc_net_close(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_net_close();
    return RET_SUCCESS;
}

static int apro_proc_net_leave(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_net_leave();
    return RET_SUCCESS;
}

static int apro_proc_net_remove_device(u32 op, char *data, u32 len)
{
    u16 *net_id = (u16*)&data[0];
    log_i("%s net_id[0x%04X]\n", __func__, *net_id);
    int st = apro_handle_remove_device(*net_id);
    if(st == RET_SUCCESS)
    {
        apro_node_del(*net_id);
    }

    if(op == EV_CMD_IPC_GET)
    {
        char buf[32] = {0,};
        u8 buf_len = 0;
        apro_ipc_payload_del_resp(buf, &buf_len, st);
        put_event(EV_IPC_SEND, EV_CMD_IPC_RESP, buf, buf_len);
    }

    return RET_SUCCESS;
}

static int apro_proc_send_unicast(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    zb_frame_t frame = {0,};
    memcpy((char*)&frame, data, sizeof(frame));

#if 0
    log_i("dest_id 0x%04X\n", frame.dest_id);
    log_i("profile_id 0x%04X\n", frame.profile_id);
    log_i("cluster_id 0x%04X\n", frame.cluster_id);
    log_i("src_ep 0x%02X\n", frame.src_ep);
    log_i("dest_ep 0x%02X\n", frame.dest_ep);
#endif

    apro_handle_send_unicast(frame);
    return RET_SUCCESS;
}

static int apro_proc_send_broadcast(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    zb_frame_t frame = {0,};
    memcpy((char*)&frame, data, sizeof(frame));

#if 0
    log_i("profile_id 0x%04X\n", frame.profile_id);
    log_i("cluster_id 0x%04X\n", frame.cluster_id);
    log_i("src_ep 0x%02X\n", frame.src_ep);
    log_i("dest_ep 0x%02X\n", frame.dest_ep);
    log_i("ash_ctrl 0x%02X\n", frame.ash_ctrl);
    log_i("cmd 0x%02X\n", frame.cmd);
    log_i("attr 0x%04X\n", frame.attr);
#endif

    apro_handle_send_broadcast(frame);
    return RET_SUCCESS;
}

static int apro_proc_send_multicast(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_send_multicast();
    return RET_SUCCESS;
}

static int apro_proc_ipc_recv(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    return RET_SUCCESS;
}

static int apro_proc_ipc_send(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_ipc_send(data, len);
    return RET_SUCCESS;
}

static int apro_proc_ocf_recv(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_ocf_parser((u8*)data, len);
    return RET_SUCCESS;
}

static int apro_proc_ocf_send(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    int ret = RET_ERROR;
    if(apro_get_ocf_st() == OCF_ST_STANDBY)
    {
        apro_ocf_send(data, len);
        ret = RET_SUCCESS;
    }
    else
    {
        log_w("%s OCF_ST_NONE\n", __func__);
    }

    return ret;
}

static int apro_proc_update_node_mgr(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_node_file_wirte();
    return RET_SUCCESS;
}

static int apro_proc_test_add_node(u32 op, char *data, u32 len)
{
    u16 *node_id = (u16*)data;
    log_i("%s node_id[0x%04x]\n", __func__, *node_id);
    u8 eui64[8] = {0,};
    apro_handle_get_node_eui64(*node_id, eui64);
    apro_node_add(*node_id, eui64);
    apro_state(ST_CMD_NEW_NODE);
    return RET_SUCCESS;
}

static int apro_proc_test_del_node(u32 op, char *data, u32 len)
{
    u16 *node_id = (u16*)data;
    log_i("%s node_id[0x%04x]\n", __func__, *node_id);
    apro_node_del(*node_id);
    return RET_SUCCESS;
}

static int apro_proc_test_dumy_node(u32 op, char *data, u32 len)
{
    log_i("%s \n", __func__);
    u16 node_id = 0x0000;
    u8 eui64[8] = {0,};
    u8 buf[8] = {0,};

    node_id = 0x1234;
    memcpy(eui64, "12341234", sizeof(eui64));
    apro_node_add(node_id, eui64);
    apro_node_update_ep(node_id, 1);
    apro_node_update_cl(node_id, 1, APRO_CL_IN, 0x0006);
    apro_node_update_latest_time(node_id);
    apro_node_update_rssi_lqi(node_id, 0xCC, -49);
    buf[0] = 0;
    apro_node_update_value(node_id, 1, 0x0006, buf, 1);

    node_id = 0x5678;
    memcpy(eui64, "56785678", sizeof(eui64));
    apro_node_add(node_id, eui64);
    apro_node_update_ep(node_id, 1);
    apro_node_update_cl(node_id, 1, APRO_CL_OUT, 0x0006);
    apro_node_update_latest_time(node_id);
    apro_node_update_rssi_lqi(node_id, 0xFF, -35);
    buf[0] = 1;
    apro_node_update_value(node_id, 1, 0x0006, buf, 1);

    node_id = 0x1122;
    memcpy(eui64, "11111111", sizeof(eui64));
    apro_node_add(node_id, eui64);
    apro_node_update_ep(node_id, 1);
    apro_node_update_cl(node_id, 1, APRO_CL_IN, 0x0402);
    apro_node_update_latest_time(node_id);
    apro_node_update_rssi_lqi(node_id, 0xFF, -31);
    buf[0] = 12;
    buf[1] = 12;
    apro_node_update_value(node_id, 1, 0x0402, buf, 2);

    node_id = 0x3344;
    memcpy(eui64, "33333333", sizeof(eui64));
    apro_node_add(node_id, eui64);
    apro_node_update_ep(node_id, 1);
    apro_node_update_cl(node_id, 1, APRO_CL_OUT, 0x0402);
    apro_node_update_latest_time(node_id);
    apro_node_update_rssi_lqi(node_id, 0xCD, -40);
    buf[0] = 23;
    buf[1] = 23;
    apro_node_update_value(node_id, 1, 0x0402, buf, 2);

    node_id = 0x5566;
    memcpy(eui64, "55555555", sizeof(eui64));
    apro_node_add(node_id, eui64);
    apro_node_update_ep(node_id, 1);
    apro_node_update_cl(node_id, 1, APRO_CL_IN, 0x0500);
    apro_node_update_latest_time(node_id);
    apro_node_update_rssi_lqi(node_id, 0xC0, -50);
    buf[0] = 0;
    apro_node_update_value(node_id, 1, 0x0500, buf, 1);

    node_id = 0x7788;
    memcpy(eui64, "77777777", sizeof(eui64));
    apro_node_add(node_id, eui64);
    apro_node_update_ep(node_id, 1);
    apro_node_update_cl(node_id, 1, APRO_CL_IN, 0x0500);
    apro_node_update_latest_time(node_id);
    apro_node_update_rssi_lqi(node_id, 0xD0, -40);
    buf[0] = 1;
    apro_node_update_value(node_id, 1, 0x0500, buf, 1);

    apro_node_file_wirte();
    apro_state(ST_CMD_NEW_NODE);
    return RET_SUCCESS;
}

static int apro_proc_aging_start(u32 op, char *data, u32 len)
{
    u16 count;
    u16 interval;
    memcpy((char*)&count, data, 2);
    memcpy((char*)&interval, (char*)&data[2], 2);
    log_i("%s count[%u] interval[%u]\n", __func__, count, interval);
    apro_test_aging_start(count, interval);
    return RET_SUCCESS;
}

static int apro_proc_aging_stop(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_test_aging_stop();
    return RET_SUCCESS;
}

static int apro_proc_reboot(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_handle_ncp_reboot();
    return RET_SUCCESS;
}

static int apro_proc_exit(u32 op, char *data, u32 len)
{
    log_i("%s\n", __func__);
    apro_state(ST_CMD_EXIT);
    apro_destroy();
    apro_handle_exit();
    return RET_SUCCESS;
}

static void apro_proc_update_time(u16 net_id)
{
    int ret = apro_node_update_latest_time(net_id);
    switch(ret)
    {
    case RET_NEW_NODE:
        {
            u8 eui64[8] = {0,};
            apro_handle_get_node_eui64(net_id, eui64);
            apro_node_add(net_id, eui64);
            apro_state(ST_CMD_NEW_NODE);
        }
        break;
    case RET_UPDATE_EP:
        apro_state(ST_CMD_NEW_NODE);
        break;

    case RET_UPDATE_CL:
        apro_state(ST_CMD_UPDATE_CL);
        break;
    }
}

static int apro_proc_cb_rst_attr(u32 op, char *data, u32 len)
{
    u8 endpoint_id = (u8)data[0];
    log_i("%s EndPoint[%d]\n", __func__, endpoint_id);
    return RET_SUCCESS;
}

static int apro_proc_cb_pre_cmd(u32 op, char *data, u32 len)
{
    cb_pre_cmd_t * frame = (cb_pre_cmd_t*)data;

    log_i("%s\n", __func__);
    log_i("NodeId[0x%04x] ProfileID[0x%04x] ClusterID[0x%04x] "
        "SrcEndPoint[0x%02x] DestEndPoint[0x%02x] "
        "Cmd[0x%02x] Options[0x%04x] GroupId[0x%02x] "
        "Sequence[0x%02x] Radius[0x%02x]",
        frame->net_id, frame->profile_id, frame->cluster_id,
        frame->src_ep, frame->dest_ep, frame->cmd_id, frame->options,
        frame->group_id, frame->sequence, frame->radius);

    int i = 0;
    debug_printf(" Data[ ");
    for(i = 0; i < frame->buf_len; i++)
    {
        debug_printf("%02x ", frame->buf[i]);
    }
    debug_printf("]\n");

    apro_proc_update_time(frame->net_id);

    switch(frame->cluster_id)
    {
    case ZCL_ON_OFF_CLUSTER_ID:
        apro_zcl_onoff_cb(data, len);
        break;

    case ZCL_ON_OFF_SWITCH_CONFIG_CLUSTER_ID:
        apro_zcl_switch_cb(data, len);
        break;

    case ZCL_LEVEL_CONTROL_CLUSTER_ID:
        apro_zcl_level_cb(data, len);
        break;

    case ZCL_TEMP_MEASUREMENT_CLUSTER_ID:
        apro_zcl_temp_cb(data, len);
        break;

    case ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID:
        apro_zcl_humid_cb(data, len);
        break;

    case ZCL_IAS_ZONE_CLUSTER_ID:
        apro_zcl_iaszone_cb(data, len);
        break;

    default:
        break;
    }

    return RET_SUCCESS;
}

static int apro_proc_cb_zdo_msg(u32 op, char *data, u32 len)
{
    cb_pre_cmd_t *frame = (cb_pre_cmd_t*)data;

    log_i("%s\n", __func__);
    log_i("NodeId[0x%04x] ProfileID[0x%04x] ClusterID[0x%04x] "
        "SrcEndPoint[0x%02x] DestEndPoint[0x%02x] "
        "Options[0x%04x] GroupId[0x%02x] Sequence[0x%02x] Radius[0x%02x]",
        frame->net_id, frame->profile_id, frame->cluster_id,
        frame->src_ep, frame->dest_ep, frame->options, frame->group_id,
        frame->sequence, frame->radius);

    int i = 0;
    debug_printf(" Data[ ");
    for(i = 0; i < frame->buf_len; i++)
    {
        debug_printf("%02x ", frame->buf[i]);
    }
    debug_printf("]\n");

    apro_proc_update_time(frame->net_id);

    switch (frame->cluster_id)
    {
    case SIMPLE_DESCRIPTOR_RESPONSE:
        {
            u8 *tran_seq = (u8*)&frame->buf[0];
            u8 *status = (u8*)&frame->buf[1];
            u16 *net_id = (u16*)&frame->buf[2];
            u8 *len = (u8*)&frame->buf[4];
            u8 *ep = (u8*)&frame->buf[5];
            u16 *profile = (u16*)&frame->buf[6];
            u16 *dev_id = (u16*)&frame->buf[8];
            u8 *ver = (u8*)&frame->buf[10];

            log_d("transSeq[0x%02x] status[0x%02x] NodeId[0x%04x] Length[%u] "
                "EndPoint[0x%02x] ProfileID[0x%04x] DeviceID[0x%04x] Version[0x%02x]\n",
                *tran_seq, *status, *net_id, *len, *ep, *profile, *dev_id, *ver);

            u8 ix = 11;
            u8 in_cl_cnt = frame->buf[ix++];
            log_d("in_cl_cnt[%u]\n", in_cl_cnt);
            if(in_cl_cnt > 0)
            {
                int i;
                for(i = 0; i < in_cl_cnt*2; i+=2)
                {
                    u16 cl_id;
                    memcpy((char*)&cl_id, (char*)&frame->buf[ix + i], 2);
                    log_d("in_cl[0x%04X]\n", cl_id);
                    apro_node_update_cl(*net_id, frame->buf[5], APRO_CL_IN, cl_id);
                }
                ix += i;
            }

            u8 out_cl_cnt = frame->buf[ix++];
            log_d("out_cl_cnt[%u]\n", out_cl_cnt);
            if(out_cl_cnt > 0)
            {
                int i;
                for(i = 0; i < out_cl_cnt*2; i+=2)
                {
                    u16 out_cl;
                    memcpy((char*)&out_cl, (char*)&frame->buf[ix + i], 2);
                    log_i("out_cl[0x%04X]\n", out_cl);
                    apro_node_update_cl(*net_id, frame->buf[5], APRO_CL_OUT, out_cl);
                }
                ix += i;
            }
        }
        break;

    case ACTIVE_ENDPOINTS_RESPONSE:
        {
            log_d("transSeq[0x%02x] status[0x%02x] NodeId[0x%02x%02x] EndPointCount[%u]\n",
                frame->buf[0], frame->buf[1], frame->buf[3], frame->buf[2], frame->buf[4]);
            if(frame->buf[4] > 0)
            {
                u16 net_id;
                memcpy((char*)&net_id, (char*)&frame->buf[2], sizeof(net_id));
                for(i = 5; i < frame->buf_len; i++)
                {
                    log_d("EndPoint[0x%02x]\n", frame->buf[i]);
                    apro_node_update_ep(net_id, frame->buf[i]);
                }
                apro_state(ST_CMD_UPDATE_CL);
            }
        }
        break;

    case END_DEVICE_ANNOUNCE:
        {
            u8 *seq = (u8*)&frame->buf[0];
            u16 *node_id = (u16*)&frame->buf[1];
            u8 *eui64 = (u8*)&frame->buf[3];
            u8 *capa = (u8*)&frame->buf[11];
            log_i("Sequence[0x%02x] NodeId[0x%04X] Capability[0x%02x]", *seq, *node_id, *capa);

            debug_printf(" Eui64[");
            int i =0;
            for(i = 0; i < 8; i++)
            {
                debug_printf("%02x ", eui64[i]);
            }
            debug_printf("]\n");

            if(apro_node_add(*node_id, eui64) == RET_SUCCESS)
            {
                apro_state(ST_CMD_NEW_NODE);
            }
        }
        break;

    case LEAVE_RESPONSE:
        {
            log_i("transSeq[0x%02x] status[0x%02x]\n", frame->buf[0], frame->buf[1]);
            if(frame->buf[1] == EMBER_ZDP_SUCCESS) // EmberZdoStatus
            {
                apro_node_del(frame->net_id);
            }
        }
        break;


    default:
        break;
    }

    return RET_SUCCESS;
}

static int apro_proc_cb_retrive_attr(u32 op, char *data, u32 len)
{
    cb_reteriv_t *reterive = (cb_reteriv_t*)data;
    log_i("%s Cluster ID[0x%04x] AttrID[0x%04x] EndPoint[0x%02x] mask[0x%02x] "
        "maunfacturer[0x%04x] read_len[0x%02x]\n",
        __func__, reterive->endpoint, reterive->cluster_id, reterive->attr_id,
        reterive->mask, reterive->maunfacturer_code, reterive->read_len);
    return RET_SUCCESS;
}

static int apro_proc_cb_key_est(u32 op, char *data, u32 len)
{
    cb_key_est_t *key = (cb_key_est_t *)data;
    log_i("%s key_status[0x%02x] eui[%X%X%X%X%X%X%X%X]\n", __func__, key->status,
        key->eui64[0], key->eui64[1], key->eui64[2], key->eui64[3],
        key->eui64[4], key->eui64[5], key->eui64[6], key->eui64[7]);

    switch(key->status)
    {
    case 0x32: // EMBER_TC_REQUESTER_VERIFY_KEY_TIMEOUT
        {
            log_i("%s net_open timeout or closed !!!\n", __func__);
            if(apro_get_web_regi() == WEB_REGI_OPEN)
            {
                char buf[32] = {0,};
                u8 buf_len = 0;
                apro_ipc_payload_regi_done(buf, &buf_len, key->status);
                put_event(EV_IPC_SEND, EV_CMD_IPC_NOTI, buf, buf_len);
                apro_set_web_regi(WEB_REGI_NONE);
            }
        }
        break;

    default:
        break;
    }

    return RET_SUCCESS;
}

static int apro_proc_cb_attr_resp(u32 op, char *data, u32 len)
{
    cb_attr_t *resp = (cb_attr_t*)data;
    log_i("%s Cluster ID: 0x%04x\n", __func__, resp->cluster_id);

    int i = 0;
    log_i("RX: ");
    for(i = 0; i < resp->buf_len; i++)
    {
      debug_printf("0x%02x ", resp->buf[i]);
    }
    debug_printf("\n");
    return RET_SUCCESS;
}

static int apro_proc_cb_report_attr(u32 op, char *data, u32 len)
{
    cb_attr_t *resp = (cb_attr_t*)data;
    log_i("%s Cluster ID: 0x%04x\n", __func__, resp->cluster_id);

    int i = 0;
    log_i("RX: ");
    for(i = 0; i < resp->buf_len; i++)
    {
      debug_printf("0x%02x ", resp->buf[i]);
    }
    debug_printf("\n");
    return RET_SUCCESS;
}

static int apro_proc_cb_send_result(u32 op, char *data, u32 len)
{
    cb_pre_cmd_t *frame = (cb_pre_cmd_t*)data;
    log_i("%s result[0x%02x] NodeId[0x%04x] ClusterID[0x%04x]",
                    __func__, frame->status, frame->net_id, frame->cluster_id);
    int i = 0;
    debug_printf(" Data[ ");
    for(i = 0; i < frame->buf_len; i++)
    {
        debug_printf("%02x ", frame->buf[i]);
    }
    debug_printf("]\n");

    if(frame->status == 0x00) // EMBER_SUCCESS
    {
        log_i("send success !!!\n");
    }
    else
    {
        log_i("send false !!!\n");
    }

    return RET_SUCCESS;
}

static int apro_proc_cb_pre_msg_recv(u32 op, char *data, u32 len)
{
    cb_rssi_t *frame = (cb_rssi_t*)data;
    log_i("%s net_id[0x%04x] lqi[0x%02x] rssi[%d]\n",
        __func__, frame->net_id, frame->lqi, frame->rssi);
    apro_node_update_rssi_lqi(frame->net_id, frame->lqi, frame->rssi);
    return RET_SUCCESS;
}

int apro_init(void)
{
    debug_init(LOG_ENABLE, LOG_INFO, "/tmp/zigbee-debug.log");
    log_i("%s\n", __func__);
    init_event();
    apro_ipc_init();
    apro_node_manager_init();
    apro_ocf_init();
    return RET_SUCCESS;
}

void apro_destroy(void)
{
    // todo free, destroy, finliize, ...
    log_i("%s\n", __func__);
    apro_ipc_deinit();
    apro_ocf_deinit();
}

typedef struct
{
    u32 cmd;                        // cmd
    int (*pf)(u32 op_code, char *data, u32 len); // function
} t_cmd;

static const t_cmd cmd_tbl[] =
{
    { EV_NOP,               apro_proc_nop               },

    { EV_CONF_VERSION,      apro_proc_version           },
    { EV_CONF_EUI64,        apro_proc_eui64             },
    { EV_CONF_PAN_ID,       apro_proc_pan_id            },
    { EV_CONF_CHANNEL,      apro_proc_channel           },
    { EV_CONF_NODE,         apro_proc_node              },
    { EV_CONF_SET,          apro_proc_set_conf          },
    { EV_CONF_NET_ST,       apro_proc_net_st            },
    { EV_CONF_NET_PARAM,    apro_proc_net_param         },
    { EV_CONF_NODE_TYPE,    apro_proc_node_type         },
    { EV_CONF_NCP,          apro_proc_ncp_config        },
    { EV_CONF_ROUTE_TABLE,  apro_proc_route_table       },
    { EV_CONF_ROUTE_SIZE,   apro_proc_route_size        },
    { EV_CONF_CHILD,        apro_proc_child             },
    { EV_CONF_NEIGHBOR,     apro_proc_neighbor          },
    { EV_CONF_NODE_INFO,    apro_proc_node_info         },
    { EV_CONF_GET_EP,       apro_proc_get_end_point     },
    { EV_CONF_GET_CL,       apro_proc_get_clustor       },

    { EV_CONF_BIND_REQ,     apro_proc_bind_request      },
    { EV_CONF_BIND_CLEAR,   apro_proc_bind_clear        },
    { EV_CONF_BIND_PRINT,   apro_proc_bind_print        },

    { EV_NET_FORM,          apro_proc_net_form          },
    { EV_NET_START,         apro_proc_net_start         },
    { EV_NET_STOP,          apro_proc_net_stop          },
    { EV_NET_OPEN,          apro_proc_net_open          },
    { EV_NET_CLOSE,         apro_proc_net_close         },
    { EV_NET_LEAVE,         apro_proc_net_leave         },
    { EV_NET_REMOVE_DEV,    apro_proc_net_remove_device },

    { EV_SEND_UNI,          apro_proc_send_unicast      },
    { EV_SEND_BR,           apro_proc_send_broadcast    },
    { EV_SEND_MULTI,        apro_proc_send_multicast    },

    { EV_IPC_RECV,          apro_proc_ipc_recv          },
    { EV_IPC_SEND,          apro_proc_ipc_send          },

    { EV_OCF_RECV,          apro_proc_ocf_recv          },
    { EV_OCF_SEND,          apro_proc_ocf_send          },

    { EV_EXE_NODE_MGR,      apro_proc_update_node_mgr   },

    { EV_TEST_ADD_NODE,     apro_proc_test_add_node     },
    { EV_TEST_DEL_NODE,     apro_proc_test_del_node     },
    { EV_TEST_DUMMY_NODE,   apro_proc_test_dumy_node    },

    { EV_TEST_AGING_START,  apro_proc_aging_start       },
    { EV_TEST_AGING_STOP,   apro_proc_aging_stop        },

    { EV_SYS_REBOOT,        apro_proc_reboot            },
    { EV_SYS_EXIT,          apro_proc_exit              },

    { EV_CB_RST_ATTR,       apro_proc_cb_rst_attr       },
    { EV_CB_PRE_CMD,        apro_proc_cb_pre_cmd        },
    { EV_CB_ZDO_MSG,        apro_proc_cb_zdo_msg        },
    { EV_CB_RETRIVE_ATTR,   apro_proc_cb_retrive_attr   },
    { EV_CB_KEY_EST,        apro_proc_cb_key_est        },
    { EV_CB_ATTR_RESP,      apro_proc_cb_attr_resp      },
    { EV_CB_REPORT_ATTR,    apro_proc_cb_report_attr    },
    { EV_CB_SEND_RESP,      apro_proc_cb_send_result    },
    { EV_CB_PRE_MSG_RCV,    apro_proc_cb_pre_msg_recv   },
};

int apro_proc(void)
{
    int ret = PROC_IDLE;
    int i;
    event_q ev;
    //log_v("%s\n", __func__);

    apro_state(ST_CMD_NONE);
    apro_ipc_proc();
    ev = handle_event();
    if(ev.event)
    {
        for(i = 0; i < sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
        {
            if(cmd_tbl[i].cmd == ev.event)
            {
                (*(cmd_tbl[i].pf))(ev.op_code, ev.data, ev.len);
                ret = PROC_CMD;
                break;
            }
        }
    }

    return ret;
}



