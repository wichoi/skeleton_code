#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "utils.h"

#include "at-handler.h"

using namespace chrono;

#ifdef LINUX_PC_APP
  #define AT_TTY_NAME   "/dev/zero"
#else
  #define AT_TTY_NAME   "/dev/ttyS2"
#endif

// AT response code
// - OK
// - ERROR
// - +CME ERROR: <errcode>

// AT response parser state
#define AT_FIND_CA_RT   0 // '\r'
#define AT_FIND_NW_LI   1 // '\n'
#define AT_FIND_1ST     2 // 'O' or 'E' or '+'
// find OK
#define AT_FIND_OK_K    3 // 'K'
#define AT_FIND_OK_CA   4 // '\r'
#define AT_FIND_OK_LI   5 // '\n'
// find ERROR
#define AT_FIND_ER_R1   6  // 'R'
#define AT_FIND_ER_R2   7  // 'R'
#define AT_FIND_ER_O    8  // 'O'
#define AT_FIND_ER_R3   9  // 'R'
#define AT_FIND_ER_CA   10 // '\r'
#define AT_FIND_ER_LI   11 // '\n'
// find +CMS ERROR: <errcode>
#define AT_FIND_CMS_C   12 // 'C'
#define AT_FIND_CMS_M   13 // 'M'
#define AT_FIND_CMS_E1  14 // 'E'
#define AT_FIND_CMS_SP  15 // ' '
#define AT_FIND_CMS_E2  16 // 'E'
#define AT_FIND_CMS_R1  17 // 'R'
#define AT_FIND_CMS_R2  18 // 'R'
#define AT_FIND_CMS_O   19 // 'O'
#define AT_FIND_CMS_R3  20 // 'R'
#define AT_FIND_CMS_CO  21 // ':'
#define AT_FIND_CMS_CA  22 // '\r'
#define AT_FIND_CMS_LI  23 // '\n'

at_handler::at_handler():
    _p_main(),
    _state(AT_START),
    _ev_q(),
    _thread(),
    _mtx(),
    _exit_flag(0),
    _ttyfd(0),
    _read_fd(),
    _at_recv(),
    _at_recv_state(AT_FIND_CA_RT),
    _resp_id()
{
}

at_handler::~at_handler()
{
    log_d("at_handler::%s\n", __func__);
    _exit_flag = 1;
}

int at_handler::init(main_interface *p_main)
{
    int ret_val = RET_OK;
    log_d("at_handler::%s \n", __func__);
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_AT_TX, this);
    _state = AT_START;
    _at_recv = "";
    _at_recv_state = AT_FIND_CA_RT;
    _resp_id = event_c::CMD_NONE;
    _thread = thread([&](){at_proc();});
    _thread.detach();
    return ret_val;
}

int at_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    if(_ttyfd != 0) close(_ttyfd);
    return RET_OK;
}

int at_handler::at_proc(void)
{
    //log_v("%s\n", __func__);
    while(_exit_flag == 0)
    {
        switch(_state)
        {
        case AT_START:
            _state = AT_INIT;
            break;
        case AT_INIT:
            if(at_init() == RET_OK)
            {
                _state = AT_IDLE;
            }
            else
            {
                this_thread::sleep_for(milliseconds(1000));
            }
            break;
        case AT_IDLE:
            at_send();
            at_recv();
            break;
        default:
            break;
        }
        this_thread::sleep_for(milliseconds(100));
        //this_thread::yield();
    }

    log_d("%s exit \n", __func__);
    return RET_OK;
}

int at_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    //log_i("at_handler::%s cmd[%d]\n", __func__, ev._cmd);
    _mtx.lock();
    if(_ev_q.size() < event_queue::QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("at_handler::%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
    return ret_val;
}

int at_handler::on_timer(u32 id)
{
    return RET_OK;
}

int at_handler::at_init(void)
{
    //log_i("%s\n", __func__);
    _ttyfd = open(AT_TTY_NAME, O_RDWR | O_NOCTTY);
    if(_ttyfd < 0)
    {
        log_e("%s tty open fail [%s]\n", __func__, AT_TTY_NAME);
        return RET_ERROR;
    }

    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;
    newtio.c_lflag     = 0;     // LF recive filter unused
    newtio.c_cc[VTIME] = 0;     // inter charater timer unused
    newtio.c_cc[VMIN]  = 0;     // blocking read until 1 character arrives

    tcflush(_ttyfd, TCIFLUSH);  // inital serial port
    tcsetattr(_ttyfd, TCSANOW, &newtio); // setting serial communication

    FD_ZERO(&_read_fd);

    log_i( "%s Opened [%s]\n", __func__, AT_TTY_NAME);
    return RET_OK;
}

int at_handler::at_send(void)
{
    event_c ev;
    ev._cmd = event_c::CMD_NONE;
    _mtx.lock();
    if(_ev_q.size() > 0)
    {
        log_d("%s : %u, op_code : %u \n",
                __func__, _ev_q.front()._cmd, _ev_q.front()._op_code);
        ev = move(_ev_q.front());
        _ev_q.pop_front();
    }
    _mtx.unlock();

    string at_stream = "";
    if(ev._cmd != event_c::CMD_NONE)
    {
        log_d("at_handler::%s cmd[%d]\n", __func__, ev._cmd);
        switch(ev._cmd)
        {
        case event_c::CMD_AT_TX:
            _at_recv.clear(); // at recv buffer clear.
            _resp_id = event_c::CMD_AT_RX;
            at_maker(ev, at_stream);
            break;
        case event_c::CMD_AT_GPS_TX:
            _at_recv.clear();
            _resp_id = event_c::CMD_AT_GPS_RX;
            at_maker(ev, at_stream);
            break;
        default:
            break;
        }
    }

    if(at_stream.length() > 0)
    {
        int ret = write(_ttyfd, at_stream.c_str(), at_stream.length());
        if(ret > 0)
        {
            log_i("%s len [%d] \n", __func__, ret);
        }
        else
        {
            log_i("%s error \n", __func__);
            if(ret < 0)
            {
                log_i("ERROR: %s\n", strerror(errno));
            }
        }
    }

    return RET_OK;
}

int at_handler::at_maker(const event_c &ev, string &stream)
{
    shared_ptr<dat_atcmd> data = static_pointer_cast<dat_atcmd>(ev._data);
    log_i("%s %s : %s \n", __func__,
                        data->atcmd.c_str(), data->stream_data.c_str());
    stream = data->stream_data;
#if 0
    // todo at cmd check, at cmd body make
    if(strcmp(data->atcmd.c_str(), "AT") == 0)
    {
    }
    else if(strcmp(data->atcmd.c_str(), "ATI") == 0)
    {
    }

    list<dat_atcmd::at_param_c>::iterator it;
    for(it = data->param.begin(); it != data->param.end(); ++it)
    {
        if(it->type == dat_atcmd::TYPE_STR)
            log_i("%s param str[%s]\n", __func__, it->value_str.c_str());
        else if(it->type == dat_atcmd::TYPE_INT)
            log_i("%s param int[%d]\n", __func__, it->value_int);
        else if(it->type == dat_atcmd::TYPE_NUM)
            log_i("%s param int[%lf]\n", __func__, it->value_num);
    }
#endif
    return RET_OK;
}

int at_handler::at_recv(void)
{
    //log_v("%s \n", __func__);
    int result;
    fd_set tmpfd;
    tmpfd = _read_fd;
    FD_SET(_ttyfd, &_read_fd);

    struct timeval tm;
    tm.tv_usec = 1000 * 300; // Quectel BG95 Maximum Response Time 300msec
    tm.tv_sec = 0;
    result = select(FD_SETSIZE, &tmpfd, NULL, NULL, &tm);
    if (result < 0)
    {
        log_e("%s fd select error !!! \n", __func__);
        return RET_ERROR;
    }
#ifdef LINUX_PC_APP
    // do nothing
#else
    if (FD_ISSET(_ttyfd, &tmpfd))
    {
        char buffer[256] = {0,};
        int sz = read(_ttyfd, buffer, sizeof(buffer));
        if(sz == -1)
            return RET_ERROR;
        log_d("%s %d : %s \n", __func__, sz, buffer);

        for(int i = 0; i < sz; i++)
        {
            _at_recv.append(&buffer[i], 1);
            switch(_at_recv_state)
            {
            case AT_FIND_CA_RT: // '\r'
                if(buffer[i] == '\r')
                {
                    _at_recv_state = AT_FIND_NW_LI;
                }
                break;
            case AT_FIND_NW_LI: // '\n'
                if(buffer[i] == '\n')
                {
                    _at_recv_state = AT_FIND_1ST;
                }
                else if(buffer[i] == '\r')
                {
                    _at_recv_state = AT_FIND_NW_LI;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_1ST: // 'O' or 'E' or '+'
                if(buffer[i] == 'O')
                {
                    _at_recv_state = AT_FIND_OK_K;
                }
                else if(buffer[i] == 'E')
                {
                    _at_recv_state = AT_FIND_ER_R1;
                }
                else if(buffer[i] == '+')
                {
                    _at_recv_state = AT_FIND_CMS_C;
                }
                else if(buffer[i] == '\r')
                {
                    _at_recv_state = AT_FIND_NW_LI;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_OK_K: // 'K'
                if(buffer[i] == 'K')
                {
                    _at_recv_state = AT_FIND_OK_CA;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_OK_CA: // '\r'
                if(buffer[i] == '\r')
                {
                    _at_recv_state = AT_FIND_OK_LI;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_OK_LI: // '\n'
                if(buffer[i] == '\n')
                {
                    log_i("%s len [%d]\n%s\n", __func__, _at_recv.size(), _at_recv.c_str());
                    at_response(_at_recv);
                    _at_recv.clear();
                }
                _at_recv_state = AT_FIND_CA_RT;
                break;
            case AT_FIND_ER_R1: // 'R'
                if(buffer[i] == 'R')
                {
                    _at_recv_state = AT_FIND_ER_R2;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_ER_R2: // 'R'
                if(buffer[i] == 'R')
                {
                    _at_recv_state = AT_FIND_ER_O;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_ER_O: // 'O'
                if(buffer[i] == 'O')
                {
                    _at_recv_state = AT_FIND_ER_R3;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_ER_R3: // 'R'
                if(buffer[i] == 'R')
                {
                    _at_recv_state = AT_FIND_ER_CA;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_ER_CA: // '\r'
                if(buffer[i] == '\r')
                {
                    _at_recv_state = AT_FIND_ER_LI;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_ER_LI: // '\n'
                if(buffer[i] == '\n')
                {
                    // todo
                    log_i("%s len [%d]\n%s\n", __func__, _at_recv.size(), _at_recv.c_str());
                    at_response(_at_recv);
                    _at_recv.clear();
                }
                _at_recv_state = AT_FIND_CA_RT;
                break;
            case AT_FIND_CMS_C: // 'C'
                if(buffer[i] == 'C')
                {
                    _at_recv_state = AT_FIND_CMS_M;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_M: // 'M'
                if(buffer[i] == 'M')
                {
                    _at_recv_state = AT_FIND_CMS_E1;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_E1: // 'E'
                if(buffer[i] == 'E')
                {
                    _at_recv_state = AT_FIND_CMS_SP;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_SP: // ' '
                if(buffer[i] == ' ')
                {
                    _at_recv_state = AT_FIND_CMS_E2;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_E2: // 'E'
                if(buffer[i] == 'E')
                {
                    _at_recv_state = AT_FIND_CMS_R1;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_R1: // 'R'
                if(buffer[i] == 'R')
                {
                    _at_recv_state = AT_FIND_CMS_R2;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_R2: // 'R'
                if(buffer[i] == 'R')
                {
                    _at_recv_state = AT_FIND_CMS_O;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_O: // 'O'
                if(buffer[i] == 'O')
                {
                    _at_recv_state = AT_FIND_CMS_R3;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_R3: // 'R'
                if(buffer[i] == 'R')
                {
                    _at_recv_state = AT_FIND_CMS_CO;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_CO: // ':'
                if(buffer[i] == ':')
                {
                    _at_recv_state = AT_FIND_CMS_CA;
                }
                else
                {
                    _at_recv_state = AT_FIND_CA_RT;
                }
                break;
            case AT_FIND_CMS_CA: // '\r'
                if(buffer[i] == '\r')
                {
                    _at_recv_state = AT_FIND_CMS_LI;
                }
                break;
            case AT_FIND_CMS_LI: // '\n'
                if(buffer[i] == '\n')
                {
                    // todo
                    log_i("%s len [%d]\n%s\n", __func__, _at_recv.size(), _at_recv.c_str());
                    at_response(_at_recv);
                    _at_recv.clear();
                }
                _at_recv_state = AT_FIND_CA_RT;
                break;
            default:
                break;
            }

            log_d("%s _at_recv_state [%d] [0x%02x]\n", __func__, _at_recv_state, buffer[i]);

            if(_at_recv.size() > 512)
            {
                log_w("%s buffer size exceed [%d] \n%s\n", __func__, _at_recv.size(), _at_recv.c_str());
                _at_recv.clear();
                _resp_id = event_c::CMD_NONE;
            }
        }
    }
#endif
    return RET_OK;
}

int at_handler::at_response(string& recv_data)
{
    log_i("%s dest_id[%d] \n", __func__, _resp_id);
    shared_ptr<dat_atcmd> data = make_shared<dat_atcmd>();
    data->stream_data = recv_data;
    _p_main->event_publish(_resp_id, event_c::OP_NONE, data);
    _resp_id = event_c::CMD_NONE;

    //data->atcmd = "ati";
    //dat_atcmd::at_param_c param1(123);
    //dat_atcmd::at_param_c param2("test");
    //data->param.push_back(param1);
    //data->param.push_back(param2);
    return RET_OK;
}

