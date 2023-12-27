#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-zcl-cmd.h"
#include "apro-zcl-switch.h"

// cluster id
// 0x0007 - ZCL_ON_OFF_SWITCH_CONFIG_CLUSTER_ID

// server

// attribute id
// 0x0000 (M) - SwitchType (enum8)          ZCL_SWITCH_TYPE_ATTRIBUTE_ID
// 0x0010 (M) - SwitchActions (enum8)       ZCL_SWITCH_ACTIONS_ATTRIBUTE_ID

int apro_zcl_switch_attr_type(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_ON_OFF_SWITCH_CONFIG_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_SWITCH_TYPE_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_SWITCH_TYPE_ATTRIBUTE_ID;

    return ret_val;
}

int apro_zcl_switch_attr_action(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_ON_OFF_SWITCH_CONFIG_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_SWITCH_ACTIONS_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_SWITCH_ACTIONS_ATTRIBUTE_ID;

    return ret_val;
}

int apro_zcl_switch_cb(char *data, u32 len)
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
                        apro_node_update_value(frame->net_id, frame->src_ep,
                            frame->cluster_id, payload.field[i].data,
                            apro_zcl_cmd_get_attr_sz(payload.field[i].data_type));

                        if(payload.field[i].attr_id == ZCL_SWITCH_TYPE_ATTRIBUTE_ID)
                        {
                            // 0x00(toggle), 0x01(momentary), 0x02(multifunction)
                            log_i("Switch Type [%d]\n", payload.field[i].data[0]);
                        }
                        else if(payload.field[i].attr_id == ZCL_SWITCH_ACTIONS_ATTRIBUTE_ID)
                        {
                            // 0x00(off), 0x01(on), 0x02(toggle)
                            log_i("Switch Actions [%d]\n", payload.field[i].data[0]);
                        }
                    }
                }
            }
        }
        break;

    default:
        break;
    }

    return ret_val;
}

