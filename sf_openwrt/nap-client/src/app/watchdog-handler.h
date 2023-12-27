#ifndef __WATCHDOG_HANDLER_H__
#define __WATCHDOG_HANDLER_H__

#include "common.h"
#include "main-interface.h"
#include "ipc-fifo.h"

class watchdog_handler :
    public main_interface,
    public event_listener,
    public ipc_fifo
{
public:
    watchdog_handler();
    ~watchdog_handler();
    int init(main_interface *p_main, u32 resolution = 100);
    int deinit(void);
    int wd_state(void);
    int wd_event(void);
    int wd_proc(void);

private:
    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

private:
    virtual int ipc_parser(ipc_pay_t *recv_data);

private:
    enum
    {
        WD_START = 0,
        WD_INIT  = 1,
        WD_IDLE  = 2
    };

private:
    main_interface *_p_main;
    int _state;
    u32 _resolution;
    list<event_c> _ev_q;
    thread _thread;
    mutex _mtx;
    int _exit_flag;
};

#endif
