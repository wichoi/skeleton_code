#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>

#include <grpcpp/grpcpp.h>

#include "log.h"
#include "utils.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "cert-manager.h"
#include "rms-handler.h"
#include "auth-handler.h"
#include "grpc-handler.h"

using namespace chrono;

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

grpc_handler::grpc_handler() :
    _p_main(),
    _ep_mgr(),
    _cert_mgr(),
    stub_msg_(),
    stub_cmd_(),
    _dl_cmd(),
    _conf_cmd(),
    _ev_q(),
    _thread(),
    _mtx(),
    _resolution(100),
    _exit_flag(0),
    _state(GRPC_ST_START),
    _unauth_ev(),
    _pending_ul(),
    _retry_config_msg(0),
    _acmd(),
    _acmd_mtx()
{
}

grpc_handler::~grpc_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int grpc_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main = p_main;

    // grpc cmd
    _p_main->event_subscribe(event_c::CMD_GRPC_START, this);
    _p_main->event_subscribe(event_c::CMD_GRPC_STOP, this);

    _ep_mgr.init();
    _cert_mgr.init();

    _dl_cmd = 0;
    _conf_cmd = 0;

    _resolution = resolution;
    _state = GRPC_ST_START;
    grpc_led_control(1);

    _unauth_ev = nullptr;
    _pending_ul.clear();
    _retry_config_msg = 0;

    _acmd.clear();

    _thread = thread([&](){grpc_proc();});
    _thread.detach();

    return ret_val;
}

int grpc_handler::deinit(void)
{
    int ret_val = RET_OK;
    log_d("grpc_handler::%s\n", __func__);
    _ep_mgr.deinit();
    _cert_mgr.deinit();
    _pending_ul.clear();
    _exit_flag = 1;
    _mtx.lock();
    _ev_q.clear();
    _mtx.unlock();
    return ret_val;
}

int grpc_handler::grpc_state(void)
{
    u32 prev_st = _state;

    return RET_OK;
}

