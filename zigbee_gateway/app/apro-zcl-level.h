#ifndef _APRO_ZCL_LEVEL_CONTROL_H_
#define _APRO_ZCL_LEVEL_CONTROL_H_

int apro_zcl_level_cmd_move_level(u16 net_id, u8 ep, u8 level, u16 time, zb_frame_t *frame);
int apro_zcl_level_cmd_move(u16 net_id, u8 ep, u8 mode, u8 rate, zb_frame_t *frame);
int apro_zcl_level_cmd_step(u16 net_id, u8 ep, u8 mode, u8 size, u16 time, zb_frame_t *frame);
int apro_zcl_level_cmd_stop(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_level_cmd_move_level_onoff(u16 net_id, u8 ep, u8 level, u16 time, zb_frame_t *frame);
int apro_zcl_level_cmd_move_onoff(u16 net_id, u8 ep, u8 mode, u8 rate, zb_frame_t *frame);
int apro_zcl_level_cmd_step_onoff(u16 net_id, u8 ep, u8 mode, u8 size, u16 time, zb_frame_t *frame);
int apro_zcl_level_cmd_stop_onoff(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_level_attr(u16 net_id, u8 ep, zb_frame_t *frame);
int apro_zcl_level_cb(char *data, u32 len);

#endif
