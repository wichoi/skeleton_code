#ifndef _APRO_IPC_PAYLOAD_H_
#define _APRO_IPC_PAYLOAD_H_

int apro_ipc_payload_version(char *buf, u32 sz, u8 major, u8 minor, u8 patch, u16 build);
int apro_ipc_payload_del_resp(char *data, u8 *len, int result);
int apro_ipc_payload_regi_done(char *data, u8 *len, int result);

#endif
