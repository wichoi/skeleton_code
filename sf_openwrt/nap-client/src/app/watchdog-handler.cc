#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "log.h"
#include "utils.h"
#include "event-data.h"
#include "event.h"
#include "config-manager.h"
#include "watchdog-handler.h"

#define IPC_PIPE_SERVER         "/tmp/ipc_watch_to_nap"
#define IPC_PIPE_CLIENT         "/tmp/ipc_nap_to_watch"

// command
#define IPC_HELLO               '0'
#define IPC_SYSTEM              '1'
#define IPC_POPEN               '2'

watchdog_handler::watchdog_handler() :
    _p_main(),
    _state(WD_START),
    _resolution(100),
    _ev_q(),
    _thread(),
    _mtx(),
    _exit_flag(0)
{
}

watchdog_handler::~watchdog_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int watchdog_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_WATCH_HELLO, this);
    _p_main->event_subscribe(event_c::CMD_WATCH_SYSTEM, this);
    _p_main->event_subscribe(event_c::CMD_WATCH_POPEN, this);

    string server = IPC_PIPE_SERVER;
    string client = IPC_PIPE_CLIENT;
    ipc_init(server, client);

    _resolution = resolution;
    _thread = thread([&](){wd_proc();});
    _thread.detach();

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int watchdog_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    ipc_deinit();
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    return RET_OK;
}

int watchdog_handler::wd_state(void)
{
    int ret_val = RET_OK;
    u32 prev_st = _state;
#ifdef LINUX_PC_APP
    // do nothing
#else
    switch(_state)
    {
    case WD_START:
        {
            _state = WD_INIT;
        }
        break;
    case WD_INIT:
        {
            _state = WD_IDLE;
        }
        break;
    case WD_IDLE:
        {
            ipc_proc();
        }
        break;
    default:
        break;
    }
#endif

    if(prev_st != _state)
    {
        log_i("watchdog_ipc::%s state[%d]\n", __func__, _state);
    }

    return ret_val;
}

int watchdog_handler::wd_event(void)
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

    if(ev._cmd != event_c::CMD_NONE)
    {
        log_d("watchdog_handler::%s cmd[%d]\n", __func__, ev._cmd);
        switch(ev._cmd)
        {
        case event_c::CMD_WATCH_HELLO:
            {
                string data = "";
                data += IPC_HELLO;
                ipc_send(data);
            }
            break;
        case event_c::CMD_WATCH_SYSTEM:
            {
                shared_ptr<dat_string> cmd = static_pointer_cast<dat_string>(ev._data);
                string data = "";
                data += IPC_SYSTEM;
                data += cmd->data;
                ipc_send(data);
            }
            break;
        case event_c::CMD_WATCH_POPEN:
            {
                shared_ptr<dat_string> cmd = static_pointer_cast<dat_string>(ev._data);
                string data = "";
                data += IPC_POPEN;
                data += cmd->data;
                ipc_send(data);
            }
            break;
        default:
            break;
        }
    }

    return RET_OK;
}

int watchdog_handler::wd_proc(void)
{
    int ret_val = RET_OK;
    while(_exit_flag == 0)
    {
        wd_state();
        wd_event();
        this_thread::sleep_for(milliseconds(_resolution));
    }
    log_d("%s exit \n", __func__);
    return ret_val;
}

int watchdog_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("watchdog_handler::%s cmd[%d]\n", __func__, ev._cmd);
    _mtx.lock();
    if(_ev_q.size() < event_queue::QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("watchdog_handler::%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
    return ret_val;
}

int watchdog_handler::ipc_parser(ipc_pay_t *recv_data)
{
    int ret_val = RET_OK;
    log_d("%s _len[%d]\n", __func__, recv_data->_len);

    switch(recv_data->_data[0])
    {
    case IPC_HELLO:
        _p_main->event_publish(event_c::CMD_WATCH_HELLO_ACK);
        break;
    case IPC_SYSTEM:
        break;
    case IPC_POPEN:
        //log_i("%s\n%s\n", __func__, &(recv_data->_data[1]));
        _p_main->event_publish(event_c::CMD_WATCH_POPEN_ACK);
        break;
    }

    return ret_val;
}

