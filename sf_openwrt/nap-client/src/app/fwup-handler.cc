#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <chrono>

#include "common.h"
#ifdef LINUX_PC_APP
  // do nothing
#else
  #include <fw-manager/fw_manager.h>
  #include <fw-manager/fw_api.h>
#endif

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "utils.h"

#include "rms-handler.h"
#include "fwup-handler.h"

using namespace chrono;

fwup_handler::fwup_handler():
    _p_main(),
    _curl_handle(),
    _resolution(100),
    _state(FW_START),
    _fw_list(),
    //_ev_q(),
    _thread(),
    //_mtx(),
    _exit_flag(0),
    _nordic_flag(FW_RESULT_NONE),
    _nordic_version()
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
    _p_main->event_subscribe(event_c::CMD_SF_FW_ACK, this);
    _p_main->event_subscribe(event_c::CMD_SF_NOTI, this);

    _curl_handle.init();

    _resolution = resolution;
    _state = FW_START;
    _fw_list.clear();

    _nordic_flag = FW_RESULT_NONE;
    _nordic_version == "";

    _thread = thread([&](){fwup_proc();});
    _thread.detach();
    return ret_val;
}

int fwup_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _curl_handle.deinit();
    //_mtx.lock();
    //_ev_q.clear();
    //_mtx.unlock();
    return RET_OK;
}

