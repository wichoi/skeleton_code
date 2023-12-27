#ifndef __APRO_IPC_H__
#define __APRO_IPC_H__

// sync  ver  flag  type  ret  src  dst  id  len  data
// (4)   (1)  (1)   (1)   (1)  (1)  (1)  (2) (2) (var)

// sync (4byte) : 0x00000001
// ver  (1byte) : version FF.FF (major.minor)
// flag (1byte) : 0x01(cbor), 0x02(json), 0x04(raw)
// type (1byte) : 0x01(request), 0x02(response), 0x04(notification)
// ret  (1byte) : 0(success), other(error)
// src  (1byte) : 1(gw launchaer) 2(z-wave) 3(zigbee) 4(ble) 5(web)
// dest (1byte) : 1(gw launchaer) 2(z-wave) 3(zigbee) 4(ble) 5(web)
// id   (2byte) : message id
// len  (2byte) : data len (network order)
// data (var  ) : variable data

#define IPC_SYNC_0              0x00
#define IPC_SYNC_1              0x00
#define IPC_SYNC_2              0x00
#define IPC_SYNC_3              0x01

#define IPC_VER                 0x00

// 0x01(cbor), 0x02(json), 0x04(raw)
#define IPC_FLAG_CBOR           0x01
#define IPC_FLAG_JSON           0x02
#define IPC_FLAG_RAW            0x04

// 0x01(request), 0x02(response), 0x04(notification)
#define IPC_TYPE_REQ            0x01
#define IPC_TYPE_RESP           0x02
#define IPC_TYPE_NOTI           0x04

// 0(success), other(error)
#define IPC_SUCC                0

// 1(gw launchaer) 2(z-wave) 3(zigbee) 4(ble) 5(web)
#define PROC_ID_GW              1
#define PROC_ID_ZW              2
#define PROC_ID_ZB              3
#define PROC_ID_BLE             4
#define PROC_ID_WEB             5

#define IPC_ZB_REGI_START       300
#define IPC_ZB_REGI_END         301
#define IPC_ZB_REGI_DONE        302
#define IPC_ZB_REGI_DEL         303
#define IPC_ZB_GET_DEV_LIST     304

int apro_ipc_init(void);
int apro_ipc_deinit(void);
int apro_ipc_send(char *send_data, u32 len);
int apro_ipc_proc(void);

#endif

