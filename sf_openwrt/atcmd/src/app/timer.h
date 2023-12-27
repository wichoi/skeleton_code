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

        // at cmd
        TID_TX_TIMEOUT          = 1000,

        // modem
        TID_MODEM_TIMEOUT       = 2000,

        // gps
        TID_GPS_FIX_TIMEOUT     = 3000,
        TID_GPS_FIX_INTERVAL    = 3001,
        TID_GPS_UPDATE          = 3002,
        TID_GPS_TIMEOUT         = 3003,
        TID_GPS_WAIT_KILLED     = 3004,

        TID_NAP_UPDATE_CONF     = 6000,

        TID_HELLO_WORLD         = 8000,

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
