#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "config-manager.h"
#include "modem-handler.h"
#include "gps-handler.h"
#include "ipc-console.h"

// sync  len  data
// (4)   (2) (var)

// sync (4byte) : sync
// len  (2byte) : data len (network order)
// data (var  ) : variable data

// syncword
#define IPC_SYNC_0          's'
#define IPC_SYNC_1          'y'
#define IPC_SYNC_2          'n'
#define IPC_SYNC_3          'c'

// state
#define IPC_ST_SYNC_0       0
#define IPC_ST_SYNC_1       1
#define IPC_ST_SYNC_2       2
#define IPC_ST_SYNC_3       3
#define IPC_ST_PID_0        4
#define IPC_ST_PID_1        5
#define IPC_ST_PID_2        6
#define IPC_ST_PID_3        7
#define IPC_ST_LEN_0        8
#define IPC_ST_LEN_1        9
#define IPC_ST_DATA         10

#define IPC_BUF_SZ          128
#define IPC_PIPE_SERVER     "/tmp/ipc_console_to_at"
#define IPC_PIPE_CLIENT     "/tmp/ipc_at_to_console"

#define UPDATE_COMMAND      "UPDATE:"
#define UPDATE_RESPONSE     "RESPONSE:"

ipc_console::ipc_console() :
    _p_main(),
    _fd_server(-1),
    _fd_client(-1),
    _ipc_st(IPC_ST_SYNC_0),
    _recv_buf(),
    _recv_head(0),
    _recv_tail(0),
    _recv_data(),
    _recv_ix(0)
{

}

ipc_console::~ipc_console()
{
    log_d("%s\n", __func__);
}

int ipc_console::init(main_interface *p_if)
{
    int ret_val = RET_OK;
    _p_main = p_if;
    _p_main->event_subscribe(event_c::CMD_AT_CON_RX, this);
    _p_main->event_subscribe(event_c::CMD_AT_UPDATE_RESP, this);

    ret_val = ipc_open_server();
    if(ret_val == RET_OK)
    {
        ret_val = ipc_open_client();
    }

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ipc_console::deinit(void)
{
    log_d("%s\n", __func__);

    close(_fd_server);
    _fd_server = -1;

    close(_fd_client);
    _fd_client = -1;

    return RET_OK;
}

int ipc_console::ipc_send(char *send_data, u32 len)
{
    int ret_val = RET_OK;
    int write_len = write(_fd_client, send_data, len);
    if(write_len < 0 || write_len != len)
    {
        ret_val = RET_ERROR;
        log_w("%s pipe write fail !!! \n", __func__);
    }
    log_d("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int ipc_console::ipc_proc(void)
{
    u8 read_buf[IPC_BUF_SZ] = {0,};
    int read_len = read(_fd_server, read_buf, sizeof(read_buf));
    //log_v("%s read_len[%d] \n", __func__, read_len);

    int i = 0;
    for(i = 0; i < read_len; i++)
    {
        _recv_buf[_recv_head++] = read_buf[i];
        if(_recv_head >= sizeof(_recv_buf))
        {
            _recv_head = 0;
        }
    }

    while(_recv_head != _recv_tail)
    {
        u8 rx = _recv_buf[_recv_tail++];
        if(_recv_tail >= sizeof(_recv_buf))
        {
            _recv_tail = 0;
        }

        switch(_ipc_st)
        {
        case IPC_ST_SYNC_0:
            if(rx == IPC_SYNC_0)
            {
                log_d("%s IPC_ST_SYNC_0[0x%02x]\n", __func__, rx);
                _ipc_st = IPC_ST_SYNC_1;
            }
            break;

        case IPC_ST_SYNC_1:
            if(rx == IPC_SYNC_1)
            {
                log_d("%s IPC_ST_SYNC_1[0x%02x]\n", __func__, rx);
                _ipc_st = IPC_ST_SYNC_2;
            }
            else if(rx == IPC_SYNC_0)
            {
                _ipc_st = IPC_ST_SYNC_1;
            }
            else
            {
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_SYNC_2:
            if(rx == IPC_SYNC_2)
            {
                log_d("%s IPC_ST_SYNC_2[0x%02x]\n", __func__, rx);
                _ipc_st = IPC_ST_SYNC_3;
            }
            else
            {
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_SYNC_3:
            if(rx == IPC_SYNC_3)
            {
                log_d("%s IPC_ST_SYNC_3[0x%02x]\n", __func__, rx);
                memset((char*)&_recv_data, 0, sizeof(_recv_data));
                _ipc_st = IPC_ST_PID_0;
            }
            else
            {
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_PID_0:
            _recv_data._pid = rx << 24;
            _ipc_st = IPC_ST_PID_1;
            break;

        case IPC_ST_PID_1:
            _recv_data._pid |= rx << 16;
            _ipc_st = IPC_ST_PID_2;
            break;

        case IPC_ST_PID_2:
            _recv_data._pid |= rx << 8;
            _ipc_st = IPC_ST_PID_3;
            break;

        case IPC_ST_PID_3:
            _recv_data._pid |= rx;
            log_d("%s pid[%d]\n", __func__, _recv_data._pid);
            _ipc_st = IPC_ST_LEN_0;
            break;

        case IPC_ST_LEN_0:
            log_d("%s data len hi[0x%02x]\n", __func__, rx);
            _recv_data._len = rx << 8;
            _ipc_st = IPC_ST_LEN_1;
            break;

        case IPC_ST_LEN_1:
            log_d("%s data len lo[0x%02x]\n", __func__, rx);
            _recv_data._len |= rx;
            _recv_ix = 0;

            if(_recv_data._len > 0)
            {
                _ipc_st = IPC_ST_DATA;
            }
            else
            {
                //ipc_parser(&_recv_data);
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        case IPC_ST_DATA:
            if(_recv_data._len > 0)
            {
                log_d("%s data [0x%02x]\n", __func__, rx);
                _recv_data._len--;
                _recv_data._data[_recv_ix++] = rx;
            }

            if(_recv_data._len == 0)
            {
                _recv_data._len = _recv_ix;
                ipc_parser(&_recv_data);
                _ipc_st = IPC_ST_SYNC_0;
            }
            break;

        default:
            log_e("Unknown IPC State !!! \n");
            _ipc_st = IPC_ST_SYNC_0;
            break;
        }
    }

    return RET_OK;
}

int ipc_console::ipc_open_server(void)
{
    int ret_val = RET_OK;

    if(_fd_server < 0)
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

        _fd_server = open(IPC_PIPE_SERVER, O_RDWR | O_NONBLOCK);
        if(_fd_server < 0)
        {
            log_e("%s fail to open pipe %s \n", __func__, IPC_PIPE_SERVER);
            ret_val = RET_ERROR;
        }
    }

    u8 read_buf[IPC_BUF_SZ] = {0,};
    while(read(_fd_server, read_buf, sizeof(read_buf)) > 0)
    {
        // fifo clear
    }

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ipc_console::ipc_open_client(void)
{
    int ret_val = RET_OK;
    if(_fd_client < 0)
    {
        int st = mkfifo(IPC_PIPE_CLIENT, 0666);
        if(st < 0) // EEXIST
        {
            log_w("%s make pipe[%d] %s \n", __func__, st, IPC_PIPE_CLIENT);
        }

        _fd_client = open(IPC_PIPE_CLIENT, O_RDWR | O_NONBLOCK);
        if(_fd_client < 0)
        {
            log_e("%s fail to open pipe %s \n", __func__, IPC_PIPE_CLIENT);
            ret_val = RET_ERROR;
        }
    }

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ipc_console::ipc_parser(ipc_pay_t *recv_data)
{
    int ret_val = RET_OK;
    log_d("%s _len[%d]\n", __func__, recv_data->_len);

    if(config_manager::instance()->get_mod_state() == modem_handler::ST_END &&
        config_manager::instance()->get_gnss_state() == gps_handler::ST_END)
    {
        shared_ptr<dat_atcmd> data = make_shared<dat_atcmd>();
        data->atcmd = "";
        data->stream_data.append((char*)recv_data->_data, (int)recv_data->_len);
        data->pid = recv_data->_pid;
        if(data->stream_data.compare(UPDATE_COMMAND) == 0 ||
            data->stream_data.compare(UPDATE_COMMAND "\r\n") == 0 )
        {
            _p_main->event_publish(event_c::CMD_AT_UPDATE_CMD);
            _p_main->event_publish(event_c::CMD_AT_UPDATE_RESP, event_c::OP_NONE, data);
        }
        else
        {
            _p_main->event_publish(event_c::CMD_AT_CON_TX, event_c::OP_NONE, data);
        }
    }
    else
    {
        shared_ptr<dat_atcmd> data = make_shared<dat_atcmd>();
        data->stream_data = "\r\nBUSY\r\n";
        _p_main->event_publish(event_c::CMD_AT_CON_RX, event_c::OP_NONE, data);
    }
    return ret_val;
}

int ipc_console::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("ipc_console::%s cmd[%d]\n", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_AT_CON_RX:
        {
            shared_ptr<dat_atcmd> data = static_pointer_cast<dat_atcmd>(ev._data);
            char *rcv_data = (char*)data->stream_data.c_str();
            u16 rcv_len = data->stream_data.length();
            if(rcv_len > 0)
            {
                char buf[256] = {0,};
                buf[0] = IPC_SYNC_0;
                buf[1] = IPC_SYNC_1;
                buf[2] = IPC_SYNC_2;
                buf[3] = IPC_SYNC_3;
                buf[4] = (data->pid >> 24) & 0xFF;
                buf[5] = (data->pid >> 16) & 0xFF;
                buf[6] = (data->pid >>  8) & 0xFF;
                buf[7] = (data->pid      ) & 0xFF;
                buf[8] = (rcv_len >> 8) & 0xFF;
                buf[9] = rcv_len & 0xFF;
                memcpy((char*)&buf[10], rcv_data, rcv_len);
                ipc_send(buf, rcv_len + 10);
            }
        }
        break;

    case event_c::CMD_AT_UPDATE_RESP:
        {
            shared_ptr<dat_atcmd> data = static_pointer_cast<dat_atcmd>(ev._data);
            char *rcv_data = (char*)UPDATE_RESPONSE;
            u16 rcv_len = strlen(rcv_data);
            if(rcv_len > 0)
            {
                char buf[256] = {0,};
                buf[0] = IPC_SYNC_0;
                buf[1] = IPC_SYNC_1;
                buf[2] = IPC_SYNC_2;
                buf[3] = IPC_SYNC_3;
                buf[4] = (data->pid >> 24) & 0xFF;
                buf[5] = (data->pid >> 16) & 0xFF;
                buf[6] = (data->pid >>  8) & 0xFF;
                buf[7] = (data->pid      ) & 0xFF;
                buf[8] = (rcv_len >> 8) & 0xFF;
                buf[9] = rcv_len & 0xFF;
                memcpy((char*)&buf[10], rcv_data, rcv_len);
                ipc_send(buf, rcv_len + 10);
            }
        }
        break;
    }

    return ret_val;
}

