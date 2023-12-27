#ifndef _APRO_ZCL_IAS_ZONE_H_
#define _APRO_ZCL_IAS_ZONE_H_

int apro_zcl_iaszone_attr(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_iaszone_cb(char *data, u32 len);

#endif
