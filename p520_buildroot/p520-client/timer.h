#ifndef __TIMER_H__
#define __TIMER_H__

#include <pthread.h>
#include <list>

#include "common.h"

class timer_listener
{
public:
    timer_listener() {}
    virtual ~timer_listener() {}

public:
    virtual int on_timer(u32 id) = 0;
};

class timer : public timer_listener
{
public:
    timer();
    ~timer();

private:
    class timer_data
    {
    public:
        u32 timer_id;
        u32 interval;
        u64 start_time;
        u64 latest_time;
        timer_listener *p_timer;
    };

public:
    int init(u32 resolution = 10);
    int deinit(void);
    int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer);
    int kill_timer(u32 id);
    int print_timer(void);
    int get_queue_cnt(void);
    int on_timer(u32 id);

public:
    int proc(void);

public:
    enum
    {
        TID_NONE                = 0,

        TID_CLOUD_RETRY         = 4000,
        TID_CLOUD_POLLING       = 4001,
        TID_CLOUD_HEARTBEAT     = 4002,
        TID_CLOUD_PARAM_UPDATED = 4003,
        TID_CLOUD_FWUP_VERIFY   = 4004,

        TID_MONITOR_SYS         = 5000,
        TID_MONITOR_NET         = 5001,
        TID_MONITOR_MEM         = 5002,

        TID_MONITOR_OTA         = 5100,

        TID_CONFIG_UPDATE       = 6000,

        TID_HELLO_WORLD         = 9000,

        TID_REBOOT              = 9000,
        TID_EXIT                = 9001,
    };

private:
    pthread_t _thread;
    pthread_mutex_t _mtx;
    list<timer_data> _tm_q;
    u32 _resolution;
    int _exit_flag;
};

#endif
