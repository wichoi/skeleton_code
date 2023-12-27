#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "log.h"
#include "ipc-fifo.h"

// sync  len  data
// (4)   (2) (var)

// sync (4byte) : sync
// len  (2byte) : data len (network order)
// data (var  ) : variable data

// syncword
#define IPC_SYNC_0              's'
#define IPC_SYNC_1              'y'
#define IPC_SYNC_2              'n'
#define IPC_SYNC_3              'c'

// state
#define IPC_ST_SYNC_0           0
#define IPC_ST_SYNC_1           1
#define IPC_ST_SYNC_2           2
#define IPC_ST_SYNC_3           3
#define IPC_ST_LEN_0            4
#define IPC_ST_LEN_1            5
#define IPC_ST_DATA             6

#define IPC_READ_SZ              256

ipc_fifo::ipc_fifo() :
    _ipc_server(-1),
    _ipc_client(-1),
    _polling_st(IPC_ST_SYNC_0),
    _poll_buf(),
    _poll_head(0),
    _poll_tail(0),
    _recv_data(),
    _recv_ix(0)
{

}

ipc_fifo::~ipc_fifo()
{
    log_d("%s\n", __func__);
}

int ipc_fifo::ipc_init(string &server, string &client)
{
    int ret_val = RET_OK;
#ifdef LINUX_PC_APP
    // do nothing
#else
    ipc_open_server(server);
    ipc_open_client(client);
#endif
    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ipc_fifo::ipc_deinit(void)
{
    log_d("%s\n", __func__);
#ifdef LINUX_PC_APP
    // do nothing
#else
    close(_ipc_server);
    _ipc_server = -1;

    close(_ipc_client);
    _ipc_client = -1;
#endif

    return RET_OK;
}

int ipc_fifo::ipc_proc(void)
{
    int ret_val = RET_OK;
#ifdef LINUX_PC_APP
    // do nothing
#else
    ipc_polling();
#endif
    return ret_val;
}

int ipc_fifo::ipc_send(string &frame)
{
#ifdef LINUX_PC_APP
    return RET_OK;
#else
    int ret_val = RET_ERROR;
    int write_len = 0;
    string sync = "sync";
    u16 body_len = (u16)frame.length();
    char len_buf[2] = {0,};

    write_len = write(_ipc_client, sync.c_str(), sync.length());
    if(write_len != sync.length())
    {
        ret_val = RET_ERROR;
        log_w("%s pipe write fail !!! [sync] \n", __func__);
        goto free_all;
    }

    len_buf[0] = (body_len >> 8) & 0xFF;
    len_buf[1] = body_len & 0xFF;
    write_len = write(_ipc_client, (char*)len_buf, 2);
    if(write_len != 2)
    {
        ret_val = RET_ERROR;
        log_w("%s pipe write fail !!! [len] \n", __func__);
        goto free_all;
    }

    write_len = write(_ipc_client, frame.c_str(), frame.length());
    if(write_len != frame.length())
    {
        ret_val = RET_ERROR;
        log_w("%s pipe write fail !!! [data] \n", __func__);
        goto free_all;
    }

    log_i("%s len[%d]\n%s\n", __func__, body_len, frame.c_str());
free_all:
    log_v("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
#endif
}

int ipc_fifo::ipc_polling(void)
{
    u8 read_buf[IPC_READ_SZ] = {0,};
    int read_len = read(_ipc_server, read_buf, sizeof(read_buf));
    //log_v("%s read_len[%d] \n", __func__, read_len);

    int i = 0;
    for(i = 0; i < read_len; i++)
    {
        _poll_buf[_poll_head++] = read_buf[i];
        if(_poll_head >= sizeof(_poll_buf))
        {
            _poll_head = 0;
        }
    }

    while(_poll_head != _poll_tail)
    {
        u8 rx = _poll_buf[_poll_tail++];
        if(_poll_tail >= sizeof(_poll_buf))
        {
            _poll_tail = 0;
        }

        switch(_polling_st)
        {
        case IPC_ST_SYNC_0:
            if(rx == IPC_SYNC_0)
            {
                log_v("%s IPC_ST_SYNC_0[0x%02x]\n", __func__, rx);
                _polling_st = IPC_ST_SYNC_1;
            }
            break;

        case IPC_ST_SYNC_1:
            if(rx == IPC_SYNC_1)
            {
                log_v("%s IPC_ST_SYNC_1[0x%02x]\n", __func__, rx);
                _polling_st = IPC_ST_SYNC_2;
            }
            else if(rx == IPC_SYNC_0)
            {
                _polling_st = IPC_ST_SYNC_1;
            }
            else
            {
                _polling_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_SYNC_2:
            if(rx == IPC_SYNC_2)
            {
                log_v("%s IPC_ST_SYNC_2[0x%02x]\n", __func__, rx);
                _polling_st = IPC_ST_SYNC_3;
            }
            else if(rx == IPC_SYNC_0)
            {
                _polling_st = IPC_ST_SYNC_1;
            }
            else
            {
                _polling_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_SYNC_3:
            if(rx == IPC_SYNC_3)
            {
                log_v("%s IPC_ST_SYNC_3[0x%02x]\n", __func__, rx);
                memset((char*)&_recv_data, 0, sizeof(_recv_data));
                _polling_st = IPC_ST_LEN_0;
            }
            else if(rx == IPC_SYNC_0)
            {
                _polling_st = IPC_ST_SYNC_1;
            }
            else
            {
                _polling_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_LEN_0:
            log_v("%s data len hi[0x%02x]\n", __func__, rx);
            _recv_data._len = rx << 8;
            _polling_st = IPC_ST_LEN_1;
            break;

        case IPC_ST_LEN_1:
            log_v("%s data len lo[0x%02x]\n", __func__, rx);
            _recv_data._len |= rx;
            _recv_ix = 0;

            if(_recv_data._len > 0)
            {
                _polling_st = IPC_ST_DATA;
            }
            else
            {
                //ipc_parser(&_recv_data);
                _polling_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_DATA:
            if(_recv_data._len > 0)
            {
                log_v("%s data [0x%02x]\n", __func__, rx);
                _recv_data._len--;
                _recv_data._data[_recv_ix++] = rx;
            }

            if(_recv_data._len == 0)
            {
                _recv_data._len = _recv_ix;
                ipc_parser(&_recv_data);
                _polling_st = IPC_ST_SYNC_0;
            }
            break;

        default:
            log_e("Unknown IPC State !!! \n");
            _polling_st = IPC_ST_SYNC_0;
            break;
        }
    }

    return RET_OK;
}

int ipc_fifo::ipc_open_server(string &server)
{
    int ret_val = RET_OK;

    if(_ipc_server < 0)
    {
        //if(access(IPC_PIPE_SERVER,F_OK) == 0)
        //{
        //    unlink(IPC_PIPE_SERVER);
        //}

        int st = mkfifo(server.c_str(), 0666);
        if(st < 0) // EEXIST
        {
            log_w("%s make pipe[%d] %s \n", __func__, st, server.c_str());
        }

        _ipc_server = open(server.c_str(), O_RDWR | O_NONBLOCK);
        if(_ipc_server < 0)
        {
            log_e("%s fail to open pipe %s \n", __func__, server.c_str());
            ret_val = RET_ERROR;
        }
    }
#if 1
    u8 read_buf[IPC_READ_SZ] = {0,};
    while(read(_ipc_server, read_buf, sizeof(read_buf)) > 0)
    {
        // fifo clear
    }
#endif
    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ipc_fifo::ipc_open_client(string &client)
{
    int ret_val = RET_OK;
    if(_ipc_client < 0)
    {
        int st = mkfifo(client.c_str(), 0666);
        if(st < 0) // EEXIST
        {
            log_w("%s make pipe[%d] %s \n", __func__, st, client.c_str());
        }

        _ipc_client = open(client.c_str(), O_RDWR | O_NONBLOCK);
        if(_ipc_client < 0)
        {
            log_e("%s fail to open pipe %s \n", __func__, client.c_str());
            ret_val = RET_ERROR;
        }
    }

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

