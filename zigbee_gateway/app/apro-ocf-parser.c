#include <string.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-node.h"
#include "apro-zcl-basic.h"
#include "apro-zcl-onoff.h"
#include "apro-zcl-temp.h"
#include "apro-zcl-humid.h"
#include "apro-zcl-iaszone.h"
#include "apro-ocf.h"
#include "apro-ocf-parser.h"

int apro_ocf_parser(u8 *data, u32 len)
{
    log_d("%s\n", __func__);
    ocf_recv_t *recv = (ocf_recv_t*)data;
#if 1 // wichoi 20201120 RS Demo
    if(recv->cnt > 0)
    {
        int i = 0;
        for(i = 0 ; i < recv->cnt; i++)
        {
            int result = 0;
            u16 net_id = 0;
            zb_frame_t frame = {0,};
            u16 dest_ep = 1;

            apro_node_ocf_get_net_id(recv->data[i].ocf_id, &net_id);
            log_i("%s net_id[0x%04x] ocf_id[%u] type[%d] val[0x%02x]\n",
                __func__, net_id, recv->data[i].ocf_id,
                recv->data[i].type, recv->data[i].value[0]);

            switch(recv->data[i].type)
            {
            case 1: //OD_Tms
                {
                    //apro_zcl_temp_attr(*dest_id, dest_ep, &frame);
                    //result = 1;
                }
                break;
            
            case 3: // OD_Door
                {
                    //apro_zcl_iaszone_attr(*dest_id, dest_ep, &frame);
                    //result = 1;
                }
                break;
            
            case 4: // OD_Motion
                {
                    //apro_zcl_iaszone_attr(*dest_id, dest_ep, &frame);
                    //result = 1;
                }
                break;
            
            case 5: // OD_Light
                {
                    if(recv->data[i].value[0] == 0)
                    {
                        apro_zcl_onoff_cmd_off(net_id, dest_ep, &frame);
                        result = 1;
                    }
                    else
                    {
                        apro_zcl_onoff_cmd_on(net_id, dest_ep, &frame);
                        result = 1;
                    }
                }
                break;
            
            default:
                break;
            }

            if(result)
            {
                put_event(EV_SEND_UNI, EV_CMD_NONE, (char*)&frame, sizeof(frame));
            }
            else
            {
                log_e("%s parsing error !!! \n", __func__);
            }
        }
    }
#endif
    return RET_SUCCESS;
}

int apro_ocf_put_event(u8 *data, u32 len)
{
    log_d("%s\n", __func__);
    put_event(EV_OCF_RECV, EV_CMD_NONE, (char*)data, len);
    return RET_SUCCESS;
}

