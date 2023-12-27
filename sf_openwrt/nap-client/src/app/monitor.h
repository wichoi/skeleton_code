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

    int mem_check(void);
    int nap_watchdog(int kick);
    int ip_change(void);
    int link_change(void);
    int gps_check(void);
    int vlan_change(void);

private:
    main_interface *_p_main;
    string _prev_ip;
    bool _prev_link;
    bool _prev_if;
    u32 _watch_cnt;
    u32 _gps_cnt;
    u32 _gps_active; // 0:none, 1:gps(lte), 2gps(wan)
    u32 _ip_null_cnt;
};

#endif
