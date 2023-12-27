#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "apro-common.h"
#include "apro-log.h"
#include "apro-event.h"
#include "apro-ipc.h"
#include "apro-ipc-parser.h"

#define IPC_SYNC_WORD_0     0
#define IPC_SYNC_WORD_1     1
#define IPC_SYNC_WORD_2     2
#define IPC_SYNC_WORD_3     3
#define IPC_VERSION         4
#define IPC_FLAG            5
#define IPC_TYPE            6
#define IPC_RESUNT          7
#define IPC_SORUCE          8
#define IPC_DESTINATION     9
#define IPC_MSG_ID_0        10
#define IPC_MSG_ID_1        11
#define IPC_DATA_LEN_0      12
#define IPC_DATA_LEN_1      13
#define IPC_DATA            14

#define IPC_MSG_SZ          128
#define IPC_PIPE_SERVER     "/tmp/ipc_zb"
#define IPC_PIPE_CLIENT     "/tmp/ipc_web"

static s32 fd_server = -1;
static s32 fd_client = -1;

static int apro_ipc_open_server(void)
{
    int ret_val = RET_SUCCESS;

    if(fd_server < 0)
    {
        //if(access(IPC_PIPE_SERVER,F_OK) == 0)
        //{
        //    unlink(IPC_PIPE_SERVER);
        //}

        int st = mkfifo(IPC_PIPE_SERVER, 0666);
        if(st < 0) // EEXIST
        {
            log_w("%s make pipe[%d] %s \n", __func__, st, IPC_PIPE_SERVER);
        }

        fd_server = open(IPC_PIPE_SERVER, O_RDWR | O_NONBLOCK);
        if(fd_server < 0)
        {
            log_e("%s fail to open pipe %s \n", __func__, IPC_PIPE_SERVER);
            ret_val = RET_ERROR;
        }
    }

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

static int apro_ipc_open_client(void)
{
    int ret_val = RET_SUCCESS;
    if(fd_client < 0)
    {
        int st = mkfifo(IPC_PIPE_CLIENT, 0666);
        if(st < 0) // EEXIST
        {
            log_w("%s make pipe[%d] %s \n", __func__, st, IPC_PIPE_CLIENT);
        }

        fd_client = open(IPC_PIPE_CLIENT, O_RDWR | O_NONBLOCK);
        if(fd_client < 0)
        {
            log_e("%s fail to open pipe %s \n", __func__, IPC_PIPE_CLIENT);
            ret_val = RET_ERROR;
        }
    }

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int apro_ipc_init(void)
{
    int ret_val = RET_SUCCESS;

    ret_val = apro_ipc_open_server();
    if(ret_val == RET_SUCCESS)
    {
        ret_val = apro_ipc_open_client();
    }

    log_i("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int apro_ipc_deinit(void)
{
    log_d("%s\n", __func__);

    close(fd_server);
    fd_server = -1;

    close(fd_client);
    fd_client = -1;

    return RET_SUCCESS;
}

int apro_ipc_send(char *send_data, u32 len)
{
    int ret_val = RET_SUCCESS;
    int write_len = write(fd_client, send_data, len);
    if(write_len < 0 || write_len != len)
    {
        ret_val = RET_ERROR;
        log_w("%s pipe write fail !!! \n", __func__);
    }
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int apro_ipc_proc(void)
{
    //log_v("%s\n", __func__);
    static u8 ipc_st = IPC_SYNC_WORD_0;
    static u8 recv_buf[Q_DATA_SZ] = {0,};
    static u16 recv_head = 0;
    static u16 recv_tail = 0;

    static ipc_pay_t recv_data;
    static u16 data_ix = 0;

    u8 read_buf[IPC_MSG_SZ] = {0,};
    int read_len = read(fd_server, read_buf, sizeof(read_buf));
    log_d("%s read_len[%d] \n", __func__, read_len);

    int i = 0;
    for(i = 0; i < read_len; i++)
    {
        recv_buf[recv_head++] = read_buf[i];
        if(recv_head >= sizeof(recv_buf))
        {
            recv_head = 0;
        }
    }

    while(recv_head != recv_tail)
    {
        u8 rx = recv_buf[recv_tail++];
        if(recv_tail >= sizeof(recv_buf))
        {
            recv_tail = 0;
        }

        switch(ipc_st)
        {
        case IPC_SYNC_WORD_0:
            if(rx == IPC_SYNC_0)
            {
                log_d("%s IPC_SYNC_WORD_0[0x%02x]\n", __func__, rx);
                ipc_st = IPC_SYNC_WORD_1;
            }
            break;

        case IPC_SYNC_WORD_1:
            if(rx == IPC_SYNC_1)
            {
                log_d("%s IPC_SYNC_WORD_1[0x%02x]\n", __func__, rx);
                ipc_st = IPC_SYNC_WORD_2;
            }
            else
            {
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        case IPC_SYNC_WORD_2:
            if(rx == IPC_SYNC_2)
            {
                log_d("%s IPC_SYNC_WORD_2[0x%02x]\n", __func__, rx);
                ipc_st = IPC_SYNC_WORD_3;
            }
            else
            {
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        case IPC_SYNC_WORD_3:
            if(rx == IPC_SYNC_3)
            {
                log_d("%s IPC_SYNC_WORD_3[0x%02x]\n", __func__, rx);
                memset((char*)&recv_data, 0, sizeof(recv_data));
                ipc_st = IPC_VERSION;
            }
            else if(rx == IPC_SYNC_0)
            {
                log_d("%s IPC_SYNC_WORD_0[0x%02x]\n", __func__, rx);
                ipc_st = IPC_SYNC_WORD_3;
            }
            else
            {
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        case IPC_VERSION:
            log_d("%s version[0x%02x]\n", __func__, rx);
            recv_data.ver = rx;
            ipc_st = IPC_FLAG;
            break;

        case IPC_FLAG:
            log_d("%s flag[0x%02x]\n", __func__, rx);
            recv_data.flag = rx;
            ipc_st = IPC_TYPE;
            break;

        case IPC_TYPE:
            log_d("%s type[0x%02x]\n", __func__, rx);
            recv_data.type = rx;
            ipc_st = IPC_RESUNT;
            break;

        case IPC_RESUNT:
            log_d("%s result[0x%02x]\n", __func__, rx);
            recv_data.ret = rx;
            ipc_st = IPC_SORUCE;
            break;

        case IPC_SORUCE:
            log_d("%s source[0x%02x]\n", __func__, rx);
            // 1(gw launchaer) 2(z-wave) 3(zigbee) 4(ble) 5(web)
            if(rx == 5)
            {
                recv_data.src = rx;
                ipc_st = IPC_DESTINATION;
            }
            else
            {
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        case IPC_DESTINATION:
            log_d("%s destination[0x%02x]\n", __func__, rx);
            // 1(gw launchaer) 2(z-wave) 3(zigbee) 4(ble) 5(web)
            if(rx == 3)
            {
                recv_data.dest = rx;
                ipc_st = IPC_MSG_ID_0;
            }
            else
            {
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        case IPC_MSG_ID_0:
            log_d("%s msg id hi[0x%02x]\n", __func__, rx);
            recv_data.msg_id = rx << 8;
            ipc_st = IPC_MSG_ID_1;
            break;

        case IPC_MSG_ID_1:
            log_d("%s msg id lo[0x%02x]\n", __func__, rx);
            recv_data.msg_id |= rx;
            ipc_st = IPC_DATA_LEN_0;
            break;

        case IPC_DATA_LEN_0:
            log_d("%s data len hi[0x%02x]\n", __func__, rx);
            recv_data.body_len = rx << 8;
            ipc_st = IPC_DATA_LEN_1;
            break;

        case IPC_DATA_LEN_1:
            log_d("%s data len lo[0x%02x]\n", __func__, rx);
            recv_data.body_len |= rx;
            data_ix = 0;

            if(recv_data.body_len > 0)
            {
                ipc_st = IPC_DATA;
            }
            else
            {
                apro_ipc_parser(&recv_data);
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        case IPC_DATA:
            if(recv_data.body_len > 0)
            {
                log_d("%s data [0x%02x]\n", __func__, rx);
                recv_data.body_len--;
                recv_data.body[data_ix++] = rx;
            }

            if(recv_data.body_len == 0)
            {
                recv_data.body_len = data_ix;
                apro_ipc_parser(&recv_data);
                ipc_st = IPC_SYNC_WORD_0;
            }
            break;

        default:
            log_e("Unknown IPC State !!! \n");
            ipc_st = IPC_SYNC_WORD_0;
            break;
        }
    }

    return RET_SUCCESS;
}

