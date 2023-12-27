#ifndef __MAIN_CLINET_H__
#define __MAIN_CLINET_H__

#include "FSPObjTypes.h"

#include "common.h"
#include "event.h"
#include "timer.h"
#include "cli.h"
#include "monitor.h"
#include "config-handler.h"
#include "fwup-handler.h"
#include "cloud-handler.h"
#include "main-interface.h"

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
    int main_proc(void);

    // event interface
    int event_subscribe(u32 cmd, event_listener *p_listener);
    int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, ev_data *obj = NULL);
    int on_event(const event_c &ev);

    // timer interface
    int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer);
    int kill_timer(u32 id);
    int on_timer(u32 id);
    int print_timer(const event_c &ev);

    // config
    int conf_read(const event_c &ev);
    int conf_write(const event_c &ev);
    int conf_update(const event_c &ev);

    int main_reboot(const event_c &ev);
    int main_factory_reset(const event_c &ev);
    int main_config_reset(const event_c &ev);
    int main_modem_reset(const event_c &ev);

    int system_call(const event_c &ev);

    int hello_world(const event_c &ev);
    int exit(const event_c &ev);

    int update_sysinfo(const event_c &ev);
    int notify_network_error(const event_c &ev);
    int notify_ip_change(const event_c &ev);
    int notify_link_change(const event_c &ev);
    int notify_link_up(const event_c &ev);

private:
    enum
    {
        ST_START        = 0,
        ST_TIME_CHECK   = 1,
        ST_INIT         = 2,
        ST_IDLE         = 3,
        ST_WAIT         = 4,
        ST_EXIT         = 5,
    };

private:
    int _main_state;
    int _main_time_check_cnt;
    event_queue _event_handle;
    timer _timer_handle;
    cli _cli_handle;
    config_handler _conf_handle;
    fwup_handler _fwup_handle;
    cloud_handler _cloud_handle;
    monitor _mon_handle;
};

#endif
