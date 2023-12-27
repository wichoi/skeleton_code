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
#include "timer.h"
#include "config-manager.h"
#include "sigfox-handler.h"

#define IPC_PIPE_SERVER         "/tmp/ipc_sigfox_to_nap"
#define IPC_PIPE_CLIENT         "/tmp/ipc_nap_to_sigfox"

// command
#define IPC_EV_NONE             0
#define IPC_EV_HELLO            1
#define IPC_EV_HELLO_ACK        2
#define IPC_EV_DOWNLINK         3
#define IPC_EV_DOWNLINK_ACK     4
#define IPC_EV_CONFIG           5
#define IPC_EV_CONFIG_ACK       6
#define IPC_EV_UPGRADE          7
#define IPC_EV_UPGRADE_ACK      8

sigfox_handler::sigfox_handler() :
    _p_main(),
    _json_mgr(),
    _resolution(100),
    _ev_q(),
    _thread(),
    _mtx(),
    _exit_flag(0),
    _state(SF_START),
    _acmd_ready(0)
{

}

sigfox_handler::~sigfox_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int sigfox_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
#ifdef LINUX_PC_APP
    // do nothing
#else
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_SF_HELLO, this);
    _p_main->event_subscribe(event_c::CMD_SF_DOWNLINK, this);
    _p_main->event_subscribe(event_c::CMD_SF_CONFIG, this);
    _p_main->event_subscribe(event_c::CMD_SF_FW_UPGRADE, this);
    _p_main->event_subscribe(event_c::CMD_SF_NOTI_ACK, this);
    _p_main->event_subscribe(event_c::CMD_SF_TEST_START, this);
    _p_main->event_subscribe(event_c::CMD_SF_TEST_STOP, this);

    _json_mgr.init();

    _state = SF_START;
    _acmd_ready = 0;

    string server = IPC_PIPE_SERVER;
    string client = IPC_PIPE_CLIENT;
    ipc_init(server, client);

    _resolution = resolution;
    _thread = thread([&](){sf_proc();});
    _thread.detach();

#endif
    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int sigfox_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
#ifdef LINUX_PC_APP
    // do nothing
#else
    ipc_deinit();
#endif
    _json_mgr.deinit();
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    return RET_OK;
}

int sigfox_handler::sf_state(void)
{
    int ret_val = RET_OK;
    u32 prev_st = _state;
#ifdef LINUX_PC_APP
    // do nothing
#else
    switch(_state)
    {
    case SF_START:
        {
            _state = SF_INIT;
        }
        break;
    case SF_INIT:
        {
            _p_main->event_publish(event_c::CMD_SF_HELLO, event_c::OP_NONE, NULL);
            sf_keep_timer();
            _state = SF_READY;
        }
        break;
    case SF_READY:
        {
            ipc_proc();
            if(_acmd_ready == 1)
            {
                sf_kill_timer();
                _state = SF_IDLE;
            }
        }
        break;
    case SF_IDLE:
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
        log_i("sigfox_handler::%s state[%d]\n", __func__, _state);
    }
    return ret_val;
}

int sigfox_handler::sf_event(void)
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
        log_d("sigfox_handler::%s cmd[%d]\n", __func__, ev._cmd);
        switch(ev._cmd)
        {
        case event_c::CMD_SF_HELLO:
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        case event_c::CMD_SF_DOWNLINK:
            if(_state >= SF_READY)
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        case event_c::CMD_SF_CONFIG:
            if(_state == SF_IDLE)
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        case event_c::CMD_SF_FW_UPGRADE:
            if(_state == SF_IDLE)
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        case event_c::CMD_SF_NOTI_ACK:
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        case event_c::CMD_SF_TEST_START:
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        case event_c::CMD_SF_TEST_STOP:
            {
                string json_body = "";
                if(_json_mgr.json_create(json_body, ev) == RET_OK)
                {
                    ipc_send(json_body);
                }
                else
                {
                    log_i("json body create failed !!!\n", __func__);
                }
            }
            break;
        default:
            break;
        }
    }
    return RET_OK;
}

int sigfox_handler::sf_proc(void)
{
    int ret_val = RET_OK;
    while(_exit_flag == 0)
    {
        sf_state();
        sf_event();
        this_thread::sleep_for(milliseconds(_resolution));
    }
    log_d("%s exit \n", __func__);
    return ret_val;
}

int sigfox_handler::on_event(const event_c &ev)
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
        log_w("sigfox_handler::%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
    return ret_val;
}

int sigfox_handler::on_timer(u32 id)
{
    //log_v("%s id[%u]\n", __func__, id);
    switch(id)
    {
    case timer::TID_SF_KEEPALIVE:
        {
            _p_main->event_publish(event_c::CMD_SF_HELLO, event_c::OP_NONE, NULL);
        }
        break;
    default:
        break;
    }

    return RET_OK;
}

int sigfox_handler::ipc_parser(ipc_pay_t *recv_data)
{
    int ret_val = RET_OK;
    log_i("%s _len[%d]\n", __func__, recv_data->_len);

    event_c ev;
    string json_data((char*)recv_data->_data, 0, (int)recv_data->_len);
    ret_val = _json_mgr.json_parse(json_data, ev);

    if(ret_val == RET_OK)
    {
        if(ev._cmd == event_c::CMD_SF_HELLO_ACK)
        {
            _acmd_ready = 1;
        }
        else if(ev._cmd == event_c::CMD_SF_NOTI)
        {
            shared_ptr<dat_ipc_result> data = static_pointer_cast<dat_ipc_result>(ev._data);
            config_manager::instance()->set_nordic_version(data->id);
            sf_config_set();
            _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
        }

        _p_main->event_publish(ev._cmd, ev._op_code, ev._data);
    }

    return ret_val;
}

int sigfox_handler::sf_keep_timer(void)
{
    _p_main->set_timer(timer::TID_SF_KEEPALIVE, 1000 * 10, this); // 10 sec
    return RET_OK;
}

int sigfox_handler::sf_kill_timer(void)
{
    _p_main->kill_timer(timer::TID_SF_KEEPALIVE);
    return RET_OK;
}

int sigfox_handler::sf_config_set(void)
{
    shared_ptr<dat_grpc_conf> data = make_shared<dat_grpc_conf>();
    data->id = "start";
    data->tx_active = (int)config_manager::instance()->get_tx_active();
    data->max_power = config_manager::instance()->get_max_power();
    data->up_freq = config_manager::instance()->get_up_freq();
    data->dn_freq = config_manager::instance()->get_dn_freq();
    data->dn_method = (dat_grpc_conf::down_method_e)config_manager::instance()->get_dn_method();
    data->lbt_conf.min = config_manager::instance()->get_lbt_min();
    data->lbt_conf.max = config_manager::instance()->get_lbt_max();
    data->lbt_conf.delay = config_manager::instance()->get_lbt_delay();
    data->lbt_conf.num = config_manager::instance()->get_lbt_num();
    data->lbt_conf.power = config_manager::instance()->get_lbt_power();
    data->longitude = config_manager::instance()->get_longitude();
    data->latitude = config_manager::instance()->get_latitude();
    _p_main->event_publish(event_c::CMD_SF_NOTI_ACK, event_c::OP_NONE, data);
    return RET_OK;
}
