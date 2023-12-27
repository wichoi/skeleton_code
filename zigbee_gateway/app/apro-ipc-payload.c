#include <stdio.h>
#include <string.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-ipc.h"
#include "apro-ipc-payload.h"

int apro_ipc_payload_version(char *buf, u32 sz, u8 major, u8 minor, u8 patch, u16 build)
{
    // todo
    // make ipc message payload
    snprintf(buf, sz - 1, "version %u.%u.%u.%u", major, minor, patch, build);
    log_i("%s %s\n", __func__, buf);
    return RET_SUCCESS;
}

int apro_ipc_payload_del_resp(char *data, u8 *len, int result)
{
    int ret_val = RET_SUCCESS;
    log_d("%s \n", __func__);

    u8 buf[32] = {0,};

    buf[0] = IPC_SYNC_0;        // sync word
    buf[1] = IPC_SYNC_1;
    buf[2] = IPC_SYNC_2;
    buf[3] = IPC_SYNC_3;
    buf[4] = IPC_VER;           // version
    buf[5] = IPC_FLAG_RAW;      // flag
    buf[6] = IPC_TYPE_RESP;     // type
    buf[7] = IPC_SUCC;          // return
    buf[8] = PROC_ID_ZB;        // src
    buf[9] = PROC_ID_WEB;       // dest
    buf[10] = (IPC_ZB_REGI_DEL >> 8) & 0xFF;    // message id (hi)
    buf[11] = (u8)(IPC_ZB_REGI_DEL & 0xFF);     // message id (lo)
    buf[12] = 0;                // data len (hi)
    buf[13] = 1;                // data len (lo)
    buf[14] = (u8)result;       // data

    memcpy((char*)data, (char*)buf, 15);
    *len =  15;

    return ret_val;
}

int apro_ipc_payload_regi_done(char *data, u8 *len, int result)
{
    int ret_val = RET_SUCCESS;
    log_d("%s \n", __func__);

    u8 buf[32] = {0,};

    buf[0] = IPC_SYNC_0;        // sync word
    buf[1] = IPC_SYNC_1;
    buf[2] = IPC_SYNC_2;
    buf[3] = IPC_SYNC_3;
    buf[4] = IPC_VER;           // version
    buf[5] = IPC_FLAG_RAW;      // flag
    buf[6] = IPC_TYPE_RESP;     // type
    buf[7] = IPC_SUCC;          // return
    buf[8] = PROC_ID_ZB;        // src
    buf[9] = PROC_ID_WEB;       // dest
    buf[10] = (IPC_ZB_REGI_DONE >> 8) & 0xFF;    // message id (hi)
    buf[11] = (u8)(IPC_ZB_REGI_DONE & 0xFF);     // message id (lo)
    buf[12] = 0;                // data len (hi)
    buf[13] = 1;                // data len (lo)
    buf[14] = (u8)result;       // data

    memcpy((char*)data, (char*)buf, 15);
    *len =  15;

    return ret_val;
}

