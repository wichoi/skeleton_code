#ifndef __TIMER_H__
#define __TIMER_H__

#include <thread>
#include <mutex>
#include <chrono>
#include <list>

#include "common.h"

using namespace chrono;

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
        steady_clock::time_point start_time;
        steady_clock::time_point latest_time;
        timer_listener *p_timer;
    };

public:
    int init(u32 resolution = 10);
    int deinit(void);
    int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer);
    int kill_timer(u32 id);
    int print_timer(void);
    int on_timer(u32 id);

private:
    int proc(void);

public:
    enum
    {
        TID_NONE                = 0,

        TID_AUTH_RETRY          = 1000,
        TID_AUTH_STANDBY        = 1001,
        TID_AUTH_NEW_TOKEN      = 1002,

        TID_SF_KEEPALIVE        = 4000,

        TID_SYS_MONITOR         = 5000,
        TID_NET_MONITOR         = 5001,
        TID_MEM_MONITOR         = 5002,

        TID_LINK_CHANGE         = 5100,

        TID_NAP_UPDATE_CONF     = 6000,

        TID_FWUP_NORDIC         = 7000,

        TID_TEST_GRPC_AGING     = 8000,

        TID_HELLO_WORLD         = 9000,

        TID_REBOOT              = 9000,
        TID_EXIT                = 9001,
    };

private:
    thread _thread;
    mutex _mtx;
    list<timer_data> _tm_q;
    u32 _resolution;
    int _exit_flag;
};

#endif
