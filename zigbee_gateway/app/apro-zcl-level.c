#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-zcl-cmd.h"
#include "apro-zcl-level.h"

// cluster id
// 0x0008 - ZCL_LEVEL_CONTROL_CLUSTER_ID

// server

// attribute id
// 0x0000 (M) - CurrentLevel (uint8)        ZCL_CURRENT_LEVEL_ATTRIBUTE_ID

// Command
// 0x00 (M) - Move to Level                 ZCL_MOVE_TO_LEVEL_COMMAND_ID
// 0x01 (M) - Move                          ZCL_MOVE_COMMAND_ID
// 0x02 (M) - Step                          ZCL_STEP_COMMAND_ID
// 0x03 (M) - Stop                          ZCL_STOP_COMMAND_ID
// 0x04 (M) - Move to Level (with On/Off)   ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID
// 0x05 (M) - Move (with On/Off)            ZCL_MOVE_WITH_ON_OFF_COMMAND_ID
// 0x06 (M) - Step (with On/Off)            ZCL_STEP_WITH_ON_OFF_COMMAND_ID
// 0x07 (M) - Stop                          ZCL_STOP_WITH_ON_OFF_COMMAND_ID

int apro_zcl_level_cmd_move_level(u16 net_id, u8 ep, u8 level, u16 time, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_MOVE_TO_LEVEL_COMMAND_ID;
    frame->msg[frame->msg_len++] = level; // level (u8)
    frame->msg[frame->msg_len++] = time >> 8; // Transition time (u16)
    frame->msg[frame->msg_len++] = time; // Transition time (u16)

    apro_node_backup_value(net_id, ep, frame->cluster_id, &level, sizeof(level));

    return ret_val;
}

int apro_zcl_level_cmd_move(u16 net_id, u8 ep, u8 mode, u8 rate, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_MOVE_COMMAND_ID;
    frame->msg[frame->msg_len++] = mode; // 0x00(up), 0x01(down)
    frame->msg[frame->msg_len++] = rate;

    return ret_val;
}

int apro_zcl_level_cmd_step(u16 net_id, u8 ep, u8 mode, u8 size, u16 time, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_STEP_COMMAND_ID;
    frame->msg[frame->msg_len++] = mode; // 0x00(up), 0x01(down)
    frame->msg[frame->msg_len++] = size;
    frame->msg[frame->msg_len++] = time >> 8;
    frame->msg[frame->msg_len++] = time;

    return ret_val;
}

int apro_zcl_level_cmd_stop(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_STOP_COMMAND_ID;

    return ret_val;
}

int apro_zcl_level_cmd_move_level_onoff(u16 net_id, u8 ep, u8 level, u16 time, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID;
    frame->msg[frame->msg_len++] = level; // level (u8)
    frame->msg[frame->msg_len++] = time >> 8; // Transition time (u16)
    frame->msg[frame->msg_len++] = time; // Transition time (u16)

    return ret_val;
}

int apro_zcl_level_cmd_move_onoff(u16 net_id, u8 ep, u8 mode, u8 rate, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_MOVE_WITH_ON_OFF_COMMAND_ID;
    frame->msg[frame->msg_len++] = mode; // 0x00(up), 0x01(down)
    frame->msg[frame->msg_len++] = rate;

    return ret_val;
}

int apro_zcl_level_cmd_step_onoff(u16 net_id, u8 ep, u8 mode, u8 size, u16 time, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_STEP_WITH_ON_OFF_COMMAND_ID;
    frame->msg[frame->msg_len++] = mode; // 0x00(up), 0x01(down)
    frame->msg[frame->msg_len++] = size;
    frame->msg[frame->msg_len++] = time >> 8;
    frame->msg[frame->msg_len++] = time;

    return ret_val;
}

int apro_zcl_level_cmd_stop_onoff(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_STOP_WITH_ON_OFF_COMMAND_ID;

    return ret_val;
}

int apro_zcl_level_attr(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_LEVEL_CONTROL_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_CURRENT_LEVEL_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_CURRENT_LEVEL_ATTRIBUTE_ID;

    return ret_val;
}

int apro_zcl_level_cb(char *data, u32 len)
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

                    if(payload.field[i].state == EMBER_ZCL_STATUS_SUCCESS)
                    {
                        if(payload.field[i].attr_id == ZCL_CURRENT_LEVEL_ATTRIBUTE_ID)
                        {
                            apro_node_update_value(frame->net_id, frame->src_ep,
                                frame->cluster_id, payload.field[i].data,
                                apro_zcl_cmd_get_attr_sz(payload.field[i].data_type));

                            // CurrentLevel 0x00 - 0xfe
                            log_i("Current Level [%d]\n", payload.field[i].data[0]);
#if 1 // wichoi 20201120 RS Demo
                            ocf_send_t ocf_pay = {0,};
                            ocf_pay.net_id = frame->net_id;
                            ocf_pay.ep = frame->src_ep;
                            ocf_pay.cluster = frame->cluster_id;
                            ocf_pay.attr = payload.field[i].attr_id;
                            ocf_pay.cmd = *cmd;
                            ocf_pay.data[0] = payload.field[i].data[0];
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
                // todo success response
                log_i("Level Control Command Success\n");
                switch(frame->buf[3])
                {
                case ZCL_MOVE_TO_LEVEL_COMMAND_ID :
                    {
                        u8 tmp[APRO_CL_DATA_SZ] = {0,};
                        u8 tmp_len = 0;
                        apro_node_restore_value(frame->net_id, frame->src_ep, frame->cluster_id, tmp, &tmp_len);
#if 1 // wichoi 20201120 RS Demo
                        ocf_send_t ocf_pay = {0,};
                        ocf_pay.net_id = frame->net_id;
                        ocf_pay.ep = frame->src_ep;
                        ocf_pay.cluster = frame->cluster_id;
                        ocf_pay.cmd = *cmd;
                        //log_i("len[%d] 0x%02x\n", tmp_len, tmp[0]);
                        memcpy((char*)ocf_pay.data, (char*)tmp, tmp_len);
                        ocf_pay.data_len = tmp_len;
                        put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
                    }
                    break;

                case ZCL_MOVE_COMMAND_ID :
                case ZCL_STEP_COMMAND_ID :
                case ZCL_STOP_COMMAND_ID :
                case ZCL_MOVE_TO_LEVEL_WITH_ON_OFF_COMMAND_ID :
                case ZCL_MOVE_WITH_ON_OFF_COMMAND_ID :
                case ZCL_STEP_WITH_ON_OFF_COMMAND_ID :
                case ZCL_STOP_WITH_ON_OFF_COMMAND_ID :
                    log_i("Not yet implemented [0x%02x] !!!\n", frame->buf[3]);
                    break;
                default:
                    log_i("Not Support Command [0x%02x] !!!\n", frame->buf[3]);
                    break;
                }
            }
            else
            {
                // todo fail response
                log_i("Level Control Command Fail\n");
            }
        }
        break;

    default:
        break;
    }

    return ret_val;
}

