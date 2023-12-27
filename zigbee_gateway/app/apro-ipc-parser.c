#include <stdio.h>
#include <string.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-config.h"
#include "apro-ipc.h"
#include "apro-ipc-parser.h"

int apro_ipc_parser(ipc_pay_t *recv_data)
{
    int ret_val = RET_SUCCESS;
    log_i("%s msg_id[%d] body_len[%d]\n", __func__,
                        recv_data->msg_id, recv_data->body_len);
    switch(recv_data->msg_id)
    {
    case IPC_ZB_REGI_START:
        put_event(EV_NET_START, EV_CMD_IPC_NONE, NULL, 0);
        put_event(EV_NOP, EV_CMD_IPC_NONE, NULL, 0);
        put_event(EV_NET_OPEN, EV_CMD_IPC_NONE, NULL, 0);
        //put_event(EV_TEST_DUMMY_NODE, EV_CMD_IPC_NONE, NULL, 0); // web test code
        apro_set_web_regi(WEB_REGI_OPEN);
        break;

    case IPC_ZB_REGI_END:
        put_event(EV_NET_CLOSE, EV_CMD_IPC_NONE, NULL, 0);
        apro_set_web_regi(WEB_REGI_NONE);
        break;

    case IPC_ZB_REGI_DONE:
        break;

    case IPC_ZB_REGI_DEL:
        {
            u16 net_id = 0x0000;
            net_id = recv_data->body[0] << 8;
            net_id += recv_data->body[1];
            log_d("%s net_id[0x%04X]\n", __func__, net_id);
            put_event(EV_NET_REMOVE_DEV, EV_CMD_IPC_GET, (char*)&net_id, sizeof(net_id));
        }
        break;

    case IPC_ZB_GET_DEV_LIST:
        put_event(EV_EXE_NODE_MGR, EV_CMD_IPC_NONE, NULL, 0);
        break;

    default:
        ret_val = RET_ERROR;
        break;
    }

    log_d("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}


