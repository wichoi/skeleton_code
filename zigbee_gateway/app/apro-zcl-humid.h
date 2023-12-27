#ifndef _APRO_ZCL_HUMID_H_
#define _APRO_ZCL_HUMID_H_


int apro_zcl_humid_attr(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_humid_cb(char *data, u32 len);



#endif
