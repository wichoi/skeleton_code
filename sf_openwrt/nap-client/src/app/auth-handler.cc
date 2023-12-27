#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>

#include "log.h"
#include "utils.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "rms-handler.h"
#include "auth-handler.h"

using namespace chrono;

auth_handler::auth_handler():
    _p_main(),
    _curl_handle(),
    _json_mgr(),
    _cert_mgr(),
    _state(AUTH_START),
    _resolution(100),
    _retry_flag(RETRY_NONE),
    _retry_cnt(0),
    _net_change(0),
    _ev_q(),
    _thread(),
    _mtx(),
    _exit_flag(0),
    _new_token_flag(0)
{
}

auth_handler::~auth_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int auth_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("auth_handler::%s \n", __func__);
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_AUTH_EXPIRED, this);
    _p_main->event_subscribe(event_c::CMD_AUTH_DENIED, this);
    _p_main->event_subscribe(event_c::CMD_AUTH_UNAUTH, this);

    _resolution = resolution;
    _state = AUTH_START;
    _retry_flag = RETRY_NONE;
    _retry_cnt = 0;
    _net_change = 0;
    _curl_handle.init();
    _cert_mgr.init();
    _json_mgr.init();

    _new_token_flag = 0;

    _thread = thread([&](){auth_proc();});
    _thread.detach();

    return ret_val;
}

int auth_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    _curl_handle.deinit();
    _cert_mgr.deinit();
    _json_mgr.deinit();
    return RET_OK;
}

int auth_handler::auth_retry(int retry)
{
    log_d("auth_standby flag[%d] cnt[%d]\n", retry, _retry_cnt);
    return RET_OK;
}

int auth_handler::auth_state(void)
{
    //log_v("%s\n", __func__);
    if(_retry_flag == RETRY_SET || _retry_flag == STANDBY_SET)
    {
        return RET_OK;
    }

    u32 prev_st = _state;

    switch(_state)
    {
    case AUTH_START:
        break;
    default:
        log_w("%s invalid state [%d]\n", __func__, _state);
        auth_retry(RETRY_NONE);
        _state = AUTH_START;
        break;
    }

    if(prev_st != _state)
    {
        config_manager::instance()->set_st_auth(_state);
    }

    return RET_OK;
}

int auth_handler::auth_event(void)
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
        case event_c::CMD_MON_IP_CHANGE:
        case event_c::CMD_MON_LINK_CHANGE:
            auth_retry(RETRY_NONE);
            _net_change = 1;
            break;
        default:
            break;
        }
    }

    return RET_OK;
}

int auth_handler::auth_proc(void)
{
    //log_v("%s\n", __func__);
    while(_exit_flag == 0)
    {
        auth_state();
        auth_event();
        this_thread::sleep_for(milliseconds(_resolution));
        //this_thread::yield();
    }

    log_d("%s exit \n", __func__);
    return RET_OK;
}

int auth_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("auth_handler::%s cmd[%d]\n", __func__, ev._cmd);
    _mtx.lock();
    if(_ev_q.size() < event_queue::QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("auth_handler::%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
    return ret_val;
}

int auth_handler::on_timer(u32 id)
{
    log_v("%s id[%u]\n", __func__, id);
    switch(id)
    {
    case timer::TID_AUTH_RETRY:
        {
            _p_main->kill_timer(timer::TID_AUTH_RETRY);
            auth_retry(RETRY_EXECUTE);
        }
        break;
    case timer::TID_AUTH_STANDBY:
        {
            _p_main->kill_timer(timer::TID_AUTH_STANDBY);
            auth_retry(STANDBY_EXECUTE);
        }
        break;
    case timer::TID_AUTH_NEW_TOKEN:
        {
            _p_main->kill_timer(timer::TID_AUTH_NEW_TOKEN);
            _new_token_flag = 0;
        }
        break;
    default:
        break;
    }
    return RET_OK;
}

int auth_handler::ca_request(void)
{
    log_d("%s\n", __func__);
    int ret_val = RET_ERROR;
    return ret_val;
}

int auth_handler::enroll_request(void)
{
    log_d("%s\n", __func__);
    int ret_val = RET_ERROR;
    return ret_val;
}

int auth_handler::re_enroll_request(void)
{
    log_d("%s\n", __func__);
    int ret_val = RET_ERROR;
    return ret_val;
}

int auth_handler::token_request(void)
{
    log_d("%s\n", __func__);
    int ret_val = RET_ERROR;
    return ret_val;
}

int auth_handler::auth_last_connection_record(string event_id, int reason)
{
    return RET_OK;
}

int auth_handler::auth_err_report(string &time, string &event_id, string &reason)
{
    return RET_OK;
}

int auth_handler::auth_clear(void)
{
    log_i("%s\n", __func__);
    _cert_mgr.auth_ca_delete();
    _cert_mgr.auth_crt_delete();
    return RET_OK;
}

