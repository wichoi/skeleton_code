#ifndef __GRPC_HANDLER_H__
#define __GRPC_HANDLER_H__

#include "cmd-service.grpc.pb.h"
#include "msg-service.grpc.pb.h"

#include "common.h"
#include "main-interface.h"
#include "ep-manager.h"

class grpc_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    grpc_handler();
    ~grpc_handler();

    int init(main_interface *p_main, u32 resolution = 100);
    int deinit(void);

private:
    int grpc_state(void);
    int grpc_event(void);
    int grpc_proc(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

    // grpc client start/stop
    int grpc_start(const event_c &ev);
    int grpc_stop(const event_c &ev);


private:
    main_interface *_p_main;
    ep_manager _ep_mgr;
    cert_manager _cert_mgr;
    unique_ptr<stbp::v1::messages::AccessPointGateway::Stub> stub_msg_;
    unique_ptr<stbp::v1::commands::AccessPointGatewayCommands::Stub> stub_cmd_;
    atomic<int> _dl_cmd;
    atomic<int> _conf_cmd;
    list<event_c> _ev_q;
    thread _thread;
    mutex _mtx;
    u32 _resolution;
    int _exit_flag;
    u32 _state;
    shared_ptr<event_c> _unauth_ev;
    list<event_c> _pending_ul;
    u32 _retry_config_msg; // 30s, 60s 120s, 240s, 240s ...

    // sigfox ipc result
    list<acmd_list_t> _acmd;
    mutex _acmd_mtx;
};

#endif
