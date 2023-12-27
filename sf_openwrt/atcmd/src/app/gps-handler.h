#ifndef __GPS_HANDLER_H__
#define __GPS_HANDLER_H__

#include <atomic>

#include "common.h"
#include "event.h"
#include "timer.h"
#include "main-interface.h"

class gps_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    gps_handler();
    ~gps_handler();
    int init(main_interface *p_if);
    int deinit(void);
    int gps_proc(void);

private:
    // event_listener
    int on_event(const event_c &ev);

    // timer_listener
    int on_timer(u32 id);

private:
    int atcmd_tx(string atcmd);

public:
    enum
    {
        ST_START        = 0,
        ST_INIT         = 1,
        ST_WAIT_KILLED  = 2,
        ST_ON_TX        = 3,
        ST_ON_RX        = 4,
        ST_CONF_TX      = 5,
        ST_CONF_RX      = 6,
        ST_LOC_TX       = 7,
        ST_LOC_RX       = 8,
        ST_OFF_TX       = 9,
        ST_OFF_RX       = 10,
        ST_ERR_TX       = 11,
        ST_ERR_RX       = 12,
        ST_CLEAR        = 13,
        ST_END          = 14
    };

private:
    enum
    {
        GNSS_READ       = 0,
        GNSS_TIMEOUT    = 1
    };

    enum
    {
        UPDATE_NONE     = 0,
        UPDATE_WAIT     = 1,
        UPDATE_EXECUTE  = 2,
    };

private:
    main_interface *_p_main;
    u32 _state;
    u32 _ret_cnt;
    atomic<int> _gnss_read_st;
    atomic<int> _gnss_read_flag;
    atomic<int> _wait_killed;
    atomic<int> _update;
    string _rx_buffer;
    u32 _onetime_init;
};

#endif
