#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <fw-api.h>

#include "common.h"

#include "log.h"
#include "event.h"
#include "timer.h"
#include "utils.h"
#include "config-manager.h"
#include "curl-handler.h"
#include "json-manager.h"
#include "fwup-handler.h"

#define FWUP_PATH           "/tmp/"
#define FWUP_FILENAME       "/tmp/firmware.img"

#define FWUP_ROUTER         "router_"
#define FWUP_MODEM          "modem_"

static void *pthread_cb(void *arg)
{
    fwup_handler *fp_fwup = (fwup_handler*)arg;
    fp_fwup->fwup_proc();
}

fwup_handler::fwup_handler():
    _p_main(),
    _curl_handle(),
    _json_handle(),
    _resolution(100),
    _state(FW_START),
    _fw_list(),
    _thread(),
    _exit_flag(0),
    _retry_cnt(0),
    _ota_type(),
    _ota_url(),
    _ota_timestamp(),
    _fw_url()
{
}

fwup_handler::~fwup_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int fwup_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("fwup_handler::%s \n", __func__);
    _p_main = p_main;

    _curl_handle.init();
    _json_handle.init();

    _resolution = resolution;
    _state = FW_START;
    _fw_list.clear();

    _retry_cnt = 0;

    _ota_type = "";
    _ota_url = "";
    _ota_timestamp = "";
    _fw_url = "";

    if(pthread_create(&_thread, NULL, pthread_cb, (void*)this) < 0)
    {
        log_e("fwup thread create failed !!!\n");
    }
    pthread_detach(_thread);

    return ret_val;
}

int fwup_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _curl_handle.deinit();
    _json_handle.deinit();
    return RET_OK;
}

int fwup_handler::fwup_state(void)
{
    u32 prev_st = _state;

    switch(_state)
    {
    case FW_START:
        fwup_clear(0);
        _state = FW_VERSION_NOTI;
        break;
    case FW_VERSION_NOTI:
        fwup_version_notify();
        _state = FW_SMS_WAIT;
        break;
    case FW_SMS_WAIT:
       if(fwup_ota_start() == RET_OK)
       {
            _state = FW_URL_REQUEST;
       }
       else
       {
           usleep(10 * 1000 * 1000); // 10sec
       }
       break;
   case FW_URL_REQUEST:
        if(fwup_ota_url() == RET_OK)
        {
            _state = FW_METADATA;
        }
        else
        {
            // ota url get failed
            fwup_clear(1);
            _state = FW_SMS_WAIT;
        }
        break;
    case FW_METADATA:
        if(fwup_ota_metadata() == RET_OK)
        {
            _state = FW_DOWNLOAD;
        }
        else
        {
            // metdata get failed
            fwup_clear(1);
            _state = FW_SMS_WAIT;
        }
        break;
    case FW_DOWNLOAD:
        if(fwup_download() == RET_OK)
        {
            //_state = FW_VERIFY;
            _state = FW_UPLOAD;
        }
        else
        {
            // download failed
            fwup_clear(1);
            _state = FW_SMS_WAIT;
        }
        break;
    case FW_VERIFY:
        if(fwup_verify() == RET_OK)
        {
            _state = FW_IMAGE_CHECK;
        }
        else
        {
            // split, verify failed
            fwup_clear(1);
            _state = FW_SMS_WAIT;
        }
        break;
    case FW_IMAGE_CHECK:
        if(fwup_image_exist(FWUP_MODEM) == RET_OK)
        {
            _state = FW_MODEM;
        }
        else
        {
            _state = FW_ROUTER;
        }
        break;
    case FW_MODEM:
        if(fwup_modem() == RET_OK)
        {
            if(fwup_image_exist(FWUP_ROUTER) == RET_OK)
            {
                _state = FW_ROUTER;
            }
            else
            {
                _state = FW_REBOOT;
            }
        }
        else
        {
            // modem upgrade failed
            //fwup_clear(1);
            //_state = FW_SMS_WAIT;
            if(fwup_image_exist(FWUP_ROUTER) == RET_OK)
            {
                _state = FW_ROUTER;
            }
            else
            {
                _state = FW_REBOOT;
            }
        }
        break;
    case FW_ROUTER:
        if(fwup_router() == RET_OK)
        {
            // router firmware write done
            _state = FW_REBOOT;
        }
        else
        {
            // router firmware write failed
            fwup_clear(1);
            //_state = FW_SMS_WAIT;
            _state = FW_REBOOT;
        }
        break;
    case FW_UPLOAD:
        {
            fwup_upload();
            _state = FW_REBOOT;
        }
        break;
    case FW_REBOOT:
        {
            string reboot_reason = "FirmwareUpdate";
            config_manager::instance()->set_cloud_reboot_reason(reboot_reason);

            _p_main->event_publish(event_c::CMD_REBOOT);
            _state = FW_END;
        }
        break;
    case FW_END:
        {
            log_i("fwup_state FW_END !!!\n");
            sleep(10);
        }
        break;
    default:
        break;
    }

    if(prev_st != _state)
    {
        string str_st = "UNKNOWN";
        if(_state == FW_START)              str_st = "FW_START";
        else if(_state == FW_VERSION_NOTI)  str_st = "FW_VERSION_NOTI";
        else if(_state == FW_SMS_WAIT)      str_st = "FW_SMS_WAIT";
        else if(_state == FW_URL_REQUEST)   str_st = "FW_URL_REQUEST";
        else if(_state == FW_METADATA)      str_st = "FW_METADATA";
        else if(_state == FW_DOWNLOAD)      str_st = "FW_DOWNLOAD";
        else if(_state == FW_VERIFY)        str_st = "FW_VERIFY";
        else if(_state == FW_IMAGE_CHECK)   str_st = "FW_IMAGE_CHECK";
        else if(_state == FW_MODEM)         str_st = "FW_MODEM";
        else if(_state == FW_ROUTER)        str_st = "FW_ROUTER";
        else if(_state == FW_UPLOAD)        str_st = "FW_UPLOAD";
        else if(_state == FW_REBOOT)        str_st = "FW_REBOOT";
        else if(_state == FW_END)           str_st = "FW_END";
        log_i("fwup_state[%d][%s]\n", _state, str_st.c_str());
    }

    return RET_OK;
}

int fwup_handler::fwup_event(void)
{
#if 0
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
#endif
    return RET_OK;
}

int fwup_handler::fwup_proc(void)
{
    while(_exit_flag == 0)
    {
        fwup_state();
        fwup_event();
        usleep(_resolution * 1000);
    }
    log_d("%s exit \n", __func__);
    return RET_OK;
}

int fwup_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    switch(ev._cmd)
    {
    //case event_c::CMD_NONE:
    //    break;
    default:
        break;
    }

#if 0
    //log_i("fwup_handler::%s cmd[%d]\n", __func__, ev._cmd);
    _mtx.lock();
    if(_ev_q.size() < event_queue::QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    _mtx.unlock();
#endif
    return ret_val;
}

int fwup_handler::on_timer(u32 id)
{
    log_v("%s id[%u] \n", __func__, id);

    switch(id)
    {
    //case timer::TID_NONE:
    //    _p_main->kill_timer(timer::TID_NONE);
    //    break;
    default:
        break;
    }

    return RET_OK;
}

