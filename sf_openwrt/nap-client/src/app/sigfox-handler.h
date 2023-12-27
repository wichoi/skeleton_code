#ifndef __SIGFOX_HANDLER_H__
#define __SIGFOX_HANDLER_H__

#include "common.h"
#include "main-interface.h"
#include "ipc-fifo.h"
#include "sigfox-json.h"

class sigfox_handler :
    public main_interface,
    public event_listener,
    public timer_listener,
    public ipc_fifo
{
public:
    sigfox_handler();
    ~sigfox_handler();
    int init(main_interface *p_main, u32 resolution = 100);
    int deinit(void);
    int sf_state(void);
    int sf_event(void);
    int sf_proc(void);

private:
    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

private:
    virtual int ipc_parser(ipc_pay_t *recv_data);

private:
    int sf_keep_timer(void);
    int sf_kill_timer(void);
    int sf_config_set(void);

private:
    enum
    {
        SF_START = 0,
        SF_INIT  = 1,
        SF_READY = 2,
        SF_IDLE  = 3
    };

private:
    main_interface *_p_main;
    sigfox_json _json_mgr;
    u32 _resolution;
    list<event_c> _ev_q;
    thread _thread;
    mutex _mtx;
    int _exit_flag;
    int _state;
    int _acmd_ready;
};

#endif
