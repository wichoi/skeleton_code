#ifndef _APRO_ZCL_ONOFF_SWITCH_H_
#define _APRO_ZCL_ONOFF_SWITCH_H_

int apro_zcl_switch_attr_type(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_switch_attr_action(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_switch_cb(char *data, u32 len);

#endif
