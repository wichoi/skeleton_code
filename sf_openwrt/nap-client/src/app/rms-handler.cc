#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "utils.h"

#include "fwup-handler.h"
#include "rms-handler.h"

using namespace chrono;

rms_handler::rms_handler():
    _p_main(),
    _curl_handle(),
    _json_mgr(),
    _cert_mgr(),
    _resolution(100),
    _state(RMS_START),
    _retry_flag(RETRY_NONE),
    _retry_cnt(0),
    _net_change(0),
    _ev_q(),
    _thread(),
    _mtx(),
    _exit_flag(0),
    _prev_url(),
    _err_list(),
    _poll_ev_id(),
    _get_list(),
    _set_list(),
    _bulk_list(),
    _wait_syscall_response(false)
{
}

rms_handler::~rms_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int rms_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("rms_handler::%s \n", __func__);
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_RMS_BOOT, this);
    _p_main->event_subscribe(event_c::CMD_WATCH_POPEN_ACK, this);

    _curl_handle.init();
    _json_mgr.init();
    _cert_mgr.init();

    _resolution = resolution;
    _state = RMS_START;
    _retry_flag = RETRY_NONE;
    _retry_cnt = 0;
    _net_change = 0;

    _thread = thread([&](){rms_proc();});
    _thread.detach();

    return ret_val;
}

int rms_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    _curl_handle.deinit();
    _json_mgr.deinit();
    _cert_mgr.deinit();
    return RET_OK;
}

int rms_handler::rms_retry(int retry)
{
    log_i("rms_standby flag[%d] cnt[%d]\n", retry, _retry_cnt);
    _retry_flag = (retry_e)retry;
    return RET_OK;
}

int rms_handler::rms_state(void)
{
    if(_retry_flag == RETRY_SET || _retry_flag == POLLING_SET)
    {
        return RET_OK;
    }

    u32 prev_st = _state;

    switch(_state)
    {
    case RMS_START:
        _state = RMS_RSA_KEY;
        break;
    default:
        break;
    }

    if(prev_st != _state)
    {
        config_manager::instance()->set_st_rms(_state);
    }

    return RET_OK;
}

int rms_handler::rms_event(void)
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
        log_d("rms_handler::%s cmd[%d]\n", __func__, ev._cmd);
        switch(ev._cmd)
        {
        case event_c::CMD_RMS_BOOT:
            rms_boot();
            break;
        default:
            break;
        }
    }
    return RET_OK;
}

int rms_handler::rms_proc(void)
{
    //log_v("%s\n", __func__);
    while(_exit_flag == 0)
    {
        rms_state();
        rms_event();
        this_thread::sleep_for(milliseconds(_resolution));
        //this_thread::yield();
    }

    log_d("%s exit \n", __func__);
    return RET_OK;
}

int rms_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    //log_i("rms_handler::%s cmd[%d]\n", __func__, ev._cmd);
    _mtx.lock();
    if(_ev_q.size() < event_queue::QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("rms_handler::%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
    return ret_val;
}

int rms_handler::on_timer(u32 id)
{
    log_v("%s id[%u] \n", __func__, id);
    switch(id)
    {
    case timer::TID_RMS_RETRY:
        {
            _p_main->kill_timer(timer::TID_RMS_RETRY);
            rms_retry(RETRY_EXECUTE);
        }
        break;
    default:
        break;
    }
    return RET_OK;
}


