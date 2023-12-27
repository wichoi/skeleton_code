#include "af.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-zcl-cmd.h"
#include "apro-zcl-temp.h"

// cluster id
// 0x0402 - ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID

// server

// attribute id
// 0x0000 (M) - MeasuredValue (u16)         ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID
// 0x0001 (M) - MinMeasuredValue (u16)      ZCL_RELATIVE_HUMIDITY_MIN_MEASURED_VALUE_ATTRIBUTE_ID
// 0x0002 (M) - MaxMeasuredValue (u16)      ZCL_RELATIVE_HUMIDITY_MAX_MEASURED_VALUE_ATTRIBUTE_ID
// 0x0003 (O) - Tolerance (u16)             ZCL_RELATIVE_HUMIDITY_TOLERANCE_ATTRIBUTE_ID

// EMBER_ZCL_REPORTING_DIRECTION_REPORTED

int apro_zcl_humid_attr(u16 net_id, u8 ep, zb_frame_t *frame)
{
    log_i("%s\n", __func__);
    int ret_val = RET_SUCCESS;
    u8 sequence = emberNextZigDevRequestSequence();

    frame->dest_id = net_id;
    frame->profile_id = HA_PROFILE_ID;
    frame->cluster_id = ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID;
    frame->src_ep = 1;
    frame->dest_ep = ep;
    frame->option = EMBER_AF_DEFAULT_APS_OPTIONS;
    frame->group_id = 0;
    frame->sequence = sequence;
    frame->radius = ZA_MAX_HOPS;

    frame->msg[frame->msg_len++] = CLUSTER_MASK_CLIENT;
    frame->msg[frame->msg_len++] = sequence;
    frame->msg[frame->msg_len++] = ZCL_READ_ATTRIBUTES_COMMAND_ID;
    frame->msg[frame->msg_len++] = ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID >> 8;
    frame->msg[frame->msg_len++] = ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID;

    return ret_val;
}

int apro_zcl_humid_cb(char *data, u32 len)
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

                    if(payload.field[i].attr_id == ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID &&
                        payload.field[i].state == EMBER_ZCL_STATUS_SUCCESS)
                    {
                        apro_node_update_value(frame->net_id, frame->src_ep,
                            frame->cluster_id, payload.field[i].data,
                            apro_zcl_cmd_get_attr_sz(payload.field[i].data_type));

                        s16 hum;
                        memcpy((char*)&hum, (char*)&payload.field[i].data[0], sizeof(hum));
                        // payload.field[i].attr_type == ZCL_INT16S_ATTRIBUTE_TYPE
                        log_i("Humidity %u\n", hum);
#if 1 // wichoi 20201120 RS Demo
                        ocf_send_t ocf_pay = {0,};
                        ocf_pay.net_id = frame->net_id;
                        ocf_pay.ep = frame->src_ep;
                        ocf_pay.cluster = frame->cluster_id;
                        ocf_pay.attr = payload.field[i].attr_id;
                        ocf_pay.cmd = *cmd;
                        memcpy((char*)&ocf_pay.data[0], (char*)&hum, sizeof(hum));
                        ocf_pay.data_len = sizeof(hum);
                        put_event(EV_OCF_SEND, EV_CMD_RESP, (char*)&ocf_pay, sizeof(ocf_pay));
#endif
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


