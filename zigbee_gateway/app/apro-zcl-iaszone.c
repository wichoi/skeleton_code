#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-zcl-cmd.h"
#include "apro-zcl-iaszone.h"

// cluster id
// 0x0006 - ZCL_IAS_ZONE_CLUSTER_ID

// server

// attribute id
// 0x0000 (M) - ZoneState (enum8)           ZCL_ZONE_STATE_ATTRIBUTE_ID
// 0x0001 (M) - ZoneType (enum16)           ZCL_ZONE_TYPE_ATTRIBUTE_ID
// 0x0002 (M) - ZoneStatus (map16)          ZCL_ZONE_STATUS_ATTRIBUTE_ID

// ZCL_ZONE_STATUS_CHANGE_NOTIFICATION_COMMAND_ID

int apro_zcl_iaszone_attr(u16 net_id, u8 ep, zb_frame_t *frame)
{
    u32 ret_val = RET_SUCCESS;
    log_i("%s\n", __func__);
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_IAS_ZONE_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_ZONE_STATUS_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_ZONE_STATUS_ATTRIBUTE_ID;

    return ret_val;
}

int apro_zcl_iaszone_cb(char *data, u32 len)
{
    u32 ret_val = RET_SUCCESS;
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
#if 1 // wichoi 20201120 RS Demo
            // door (RS sensor)
            // close - Data[ 09 05 00 04 00 00 0X 00 00 ]
            // open  - Data[ 09 05 00 05 00 00 0X 00 00 ]

            // motion (RS sensor)
            // Data[ 09 03 00 04 00 00 0X 00 00 ]
            // Data[ 09 04 00 05 00 00 0X 00 00 ]

            u8 door = 1;
            if(door)
            {
                if(frame->buf[3] == 0x04)
                {
                    log_d("DOOR CLOSE !!!\n");
                }
                else
                {
                    log_d("DOOR OPEN !!!\n");
                }
            }
            else
            {
                if(/*frame->buf[3] == 0x04 ||*/
                    frame->buf[3] == 0x05 )
                {
                    log_d("MOTION DETECTOR !!!\n");
                }
                else
                {
                    log_d("MOTION DETECTOR Back to Normal !!!\n");
                }
            }
            ocf_send_t ocf_pay = {0,};
            ocf_pay.net_id = frame->net_id;
            ocf_pay.ep = frame->src_ep;
            ocf_pay.cluster = frame->cluster_id;
            //ocf_pay.attr = *attr_id;
            //ocf_pay.cmd = *cmd;

            if(frame->buf[3] == 0x05)
            {
                ocf_pay.data[0] = 1;
            }
            else
            {
                ocf_pay.data[0] = 0;
            }
            ocf_pay.data_len = 1;

            apro_node_update_value(frame->net_id, frame->src_ep,
                ZCL_IAS_ZONE_CLUSTER_ID, ocf_pay.data, 1);
            put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
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

                    if(payload.field[i].attr_id == ZCL_ZONE_STATUS_ATTRIBUTE_ID &&
                        payload.field[i].state == EMBER_ZCL_STATUS_SUCCESS)
                    {
                        //apro_node_update_value(frame->net_id, frame->src_ep,
                        //    frame->cluster_id, payload.field[i].data,
                        //    apro_zcl_cmd_get_attr_sz(payload.field[i].data_type));

                        if(payload.field[i].data[0] == 0)
                        {
                            log_d("0 – closed or not alarmed !!!\n");
                        }
                        else
                        {
                            log_d("1 – opened or alarmed !!!\n");
                        }
                    }
                }
            }
        }
        break;

    case ZCL_REPORT_ATTRIBUTES_COMMAND_ID:
        {
            rpt_attr_r payload;
            apro_zcl_cmd_rpt_attr(ptr, ptr_len, &payload);
            if(payload.cnt > 0)
            {
                int i = 0;
                for(i = 0; i < payload.cnt; i++)
                {
                    log_d("attr[%04x] type[%02x] value[%02x]\n",
                        payload.field[i].attr_id,
                        payload.field[i].type,
                        payload.field[i].data[0]);

                    if(payload.field[i].attr_id == ZCL_ZONE_STATUS_ATTRIBUTE_ID)
                    {
                        if(payload.field[i].data[0] == 0)
                        {
                            log_d("0 – closed or not alarmed !!!\n");
                        }
                        else
                        {
                            log_d("1 – opened or alarmed !!!\n");
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

