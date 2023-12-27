#ifndef __MONITOR_H__
#define __MONITOR_H__

#include "common.h"
#include "event.h"
#include "timer.h"
#include "main-interface.h"

class monitor :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    monitor();
    ~monitor();
    int init(main_interface *p_main);
    int deinit(void);

private:
    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

    int sys_info(void);
    int mem_check(void);
    int ip_change(void);
    int link_change(void);
    int ota_reset(void);
    int ota_sms(void);

private:
    main_interface *_p_main;
    string _prev_ip;
    bool _prev_link;
    bool _prev_if;
    u32 _ip_null_cnt;
};

#endif
