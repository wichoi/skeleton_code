#ifndef _APRO_ZCL_ONOFF_H_
#define _APRO_ZCL_ONOFF_H_

int apro_zcl_onoff_cmd_off(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_onoff_cmd_on(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_onoff_cmd_toggle(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_onoff_attr(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_onoff_cb(char *data, u32 len);

#endif
