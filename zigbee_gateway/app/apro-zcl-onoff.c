#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-zcl-cmd.h"
#include "apro-zcl-onoff.h"

// cluster id
// 0x0006 - ZCL_ON_OFF_CLUSTER_ID

// server

// attribute id
// 0x0000 (M) - OnOff (bool)                ZCL_ON_OFF_ATTRIBUTE_ID
// 0x4000 (O) - GlobalSceneControl (bool)   ZCL_GLOBAL_SCENE_CONTROL_ATTRIBUTE_ID
// 0x4001 (O) - OnTime (u16)                ZCL_ON_TIME_ATTRIBUTE_ID
// 0x4002 (O) - OffWaitTime (u16)           ZCL_OFF_WAIT_TIME_ATTRIBUTE_ID

// command
// 0x00 (M) - off                           ZCL_OFF_COMMAND_ID
// 0x01 (M) - on                            ZCL_ON_COMMAND_ID
// 0x02 (M) - toggle                        ZCL_TOGGLE_COMMAND_ID
// 0x40 (O) - Off with effect               ZCL_OFF_WITH_EFFECT_COMMAND_ID
// 0x41 (O) - On with recall global scene   ZCL_ON_WITH_RECALL_GLOBAL_SCENE_COMMAND_ID
// 0x42 (O) - On with timed off             ZCL_ON_WITH_TIMED_OFF_COMMAND_ID

// payload - todo

int apro_zcl_onoff_cmd_off(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_OFF_COMMAND_ID;

    return ret_val;
}

int apro_zcl_onoff_cmd_on(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_ON_COMMAND_ID;

    return ret_val;
}

int apro_zcl_onoff_cmd_toggle(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_TOGGLE_COMMAND_ID;

    return ret_val;
}

int apro_zcl_onoff_attr(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_ON_OFF_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_ON_OFF_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_ON_OFF_ATTRIBUTE_ID;

    // { ASH_CONTROL_ACK, sequence, Cmd ID(command-id.h), Attr-ID hi, Attr-ID low(attribute-id.h) }
    // response { FC, seq, cmd, attr hi, attr low, status(EmberAfStatus), data-type(attribute-type.h), data }

    return ret_val;
}

int apro_zcl_onoff_cb(char *data, u32 len)
{
    int ret_val = RET_SUCCESS;
    cb_pre_cmd_t * frame = (cb_pre_cmd_t*)data;

    u8 *fc = (u8*)&frame->buf[0];
    u8 *seq = (u8*)&frame->buf[1];
    u8 *cmd = (u8*)&frame->buf[2];
    u8 *ptr = (u8*)&frame->buf[3];
    u32 ptr_len = frame->buf_len - 3;
    log_d("%s fc[0x%02x] seq[0x%02x] cmd[0x%02x]\n", __func__, *fc, *seq, *cmd);

    switch(frame->cmd_id)
    {
    case ZCL_READ_ATTRIBUTES_COMMAND_ID:
        {
            rd_attr_t payload = {0,};
            apro_zcl_cmd_rd_attr(ptr, ptr_len, &payload);
        }
        break;

    case ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID:
        {
            rd_resp_t payload;
            apro_zcl_cmd_rd_attr_resp(ptr, ptr_len, &payload);
            if(payload.cnt > 0)
            {
                int i = 0;
                for(i = 0; i < payload.cnt; i++)
                {
                    log_d("attr[%04x] st[%02x] type[%02x] value[%02x]\n",
                        payload.field[i].attr_id, payload.field[i].state,
                        payload.field[i].data_type, payload.field[i].data[0]);

                    if(payload.field[i].attr_id == ZCL_ON_OFF_ATTRIBUTE_ID &&
                        payload.field[i].state == EMBER_ZCL_STATUS_SUCCESS)
                    {
                        apro_node_update_value(frame->net_id, frame->src_ep,
                            frame->cluster_id, payload.field[i].data,
                            apro_zcl_cmd_get_attr_sz(payload.field[i].data_type));

                        // payload.field[i].data_type == ZCL_BOOLEAN_ATTRIBUTE_TYPE
                        if(payload.field[i].data[0] == ZCL_OFF_COMMAND_ID)
                        {
                            // todo led off response
                            log_i("LED Off\n");
#if 1 // wichoi 20201120 RS Demo
                            ocf_send_t ocf_pay = {0,};
                            ocf_pay.net_id = frame->net_id;
                            ocf_pay.ep = frame->src_ep;
                            ocf_pay.cluster = frame->cluster_id;
                            ocf_pay.attr = payload.field[i].attr_id;
                            ocf_pay.cmd = *cmd;
                            ocf_pay.data[0] = 0;
                            ocf_pay.data_len = 1;
                            put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
                        }
                        else
                        {
                            // todo led on response
                            log_i("LED On\n");
#if 1 // wichoi 20201120 RS Demo
                            ocf_send_t ocf_pay = {0,};
                            ocf_pay.net_id = frame->net_id;
                            ocf_pay.ep = frame->src_ep;
                            ocf_pay.cluster = frame->cluster_id;
                            ocf_pay.attr = payload.field[i].attr_id;
                            ocf_pay.cmd = *cmd;
                            ocf_pay.data[0] = 1;
                            ocf_pay.data_len = 1;
                            put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
                        }
                    }
                }
            }
        }
        break;

    case ZCL_DEFAULT_RESPONSE_COMMAND_ID:
        {
            log_d("FlowControl[0x%02x] Sequence[0x%02x] Command[0x%02x] payload[%02x %02x]\n",
                frame->buf[0], frame->buf[1], frame->buf[2], frame->buf[3], frame->buf[4]);
            if(frame->buf[4] == EMBER_ZCL_STATUS_SUCCESS)
            {
                switch(frame->buf[3])
                {
                case ZCL_OFF_COMMAND_ID:
                    {
                        log_i("Off Command Success\n");
                        u8 off = 0;
                        apro_node_update_value(frame->net_id, frame->src_ep,
                            frame->cluster_id, &off, 1);
#if 1 // wichoi 20201120 RS Demo
                        ocf_send_t ocf_pay = {0,};
                        ocf_pay.net_id = frame->net_id;
                        ocf_pay.ep = frame->src_ep;
                        ocf_pay.cluster = frame->cluster_id;
                        ocf_pay.cmd = *cmd;
                        ocf_pay.data[0] = 0;
                        ocf_pay.data_len = 1;
                        put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
                    }
                    break;
                case ZCL_ON_COMMAND_ID:
                    {
                        log_i("On Command Success\n");
                        u8 on = 1;
                        apro_node_update_value(frame->net_id, frame->src_ep,
                            frame->cluster_id, &on, 1);
#if 1 // wichoi 20201120 RS Demo
                        ocf_send_t ocf_pay = {0,};
                        ocf_pay.net_id = frame->net_id;
                        ocf_pay.ep = frame->src_ep;
                        ocf_pay.cluster = frame->cluster_id;
                        ocf_pay.cmd = *cmd;
                        ocf_pay.data[0] = 1;
                        ocf_pay.data_len = 1;
                        put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
                    }
                    break;
                case ZCL_TOGGLE_COMMAND_ID:
                    log_i("Toggle Command Success\n");
                    break;
                default:
                    log_i("Not Support Command [0x%02x] !!!\n", frame->buf[3]);
                    break;
                }
            }
            else
            {
                // todo fail response
                log_i("On/Off Command Fail\n");
            }
        }
        break;

    default:
        break;
    }

    return ret_val;
}

