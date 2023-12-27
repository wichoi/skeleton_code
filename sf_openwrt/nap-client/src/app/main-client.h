#ifndef __NAP_CLINET_H__
#define __NAP_CLINET_H__

#include "common.h"
#include "event.h"
#include "timer.h"
#include "main-interface.h"
#include "cli.h"
#include "config-handler.h"
#include "auth-handler.h"
#include "grpc-handler.h"
#include "rms-handler.h"
#include "fwup-handler.h"

#include "at-handler.h"
#include "sigfox-handler.h"
#include "watchdog-handler.h"
#include "monitor.h"

class main_client :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    main_client();
    ~main_client();
    int proc(void);

private:
    int init(void);
    int deinit(const event_c &ev);
    int nap_proc(void);

    // event interface
    int event_subscribe(u32 cmd, event_listener *p_listener);
    int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, shared_ptr<dat_c> data = NULL);
    int on_event(const event_c &ev);

    // timer interface
    int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer);
    int kill_timer(u32 id);
    int on_timer(u32 id);
    int print_timer(const event_c &ev);

    // config
    int nap_conf_read(const event_c &ev);
    int nap_conf_write(const event_c &ev);
    int nap_conf_update(const event_c &ev);

    // reboot
    int nap_reboot(const event_c &ev);
    int nap_factory_reset(const event_c &ev);
    int nap_config_reset(const event_c &ev);

    // execute event
    int hello_world(const event_c &ev);
    int exit(const event_c &ev);

private:
    enum
    {
        ST_START        = 0,
        ST_INIT         = 1,
        ST_STANDBY      = 2,
        ST_WAIT         = 3,
        ST_EXIT         = 4,
    };

private:
    int _nap_state;
    event_queue _event_handle;
    timer _timer_handle;
    cli _cli_handle;
    config_handler _conf_handler;
    auth_handler _auth_handle;
    grpc_handler _grpc_handle;
    rms_handler _rms_handle;
    fwup_handler  _fwup_handle;
    //at_handler _at_handle;
    sigfox_handler _sf_handle;
    watchdog_handler _wd_handle;
    monitor _mon_handle;
};

#endif
