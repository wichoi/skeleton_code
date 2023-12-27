#ifndef __AT_HANDLER_H__
#define __AT_HANDLER_H__

#include <sys/select.h>

#include "common.h"
#include "main-interface.h"

class at_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    at_handler();
    ~at_handler();
    int init(main_interface *p_if);
    int deinit(void);

private:
    int at_proc(void);

    // event_listener
    int on_event(const event_c &ev);

    // timer_listener
    int on_timer(u32 id);

    int at_init(void);
    int at_send(void);
    int at_maker(const event_c &ev, string &stream);
    int at_polling(void);
    int at_resp(string& recv_data);
    int at_urc(string& recv_data);

public:
    enum
    {
        AT_START            = 0,
        AT_INIT             = 1,
        AT_IDLE             = 2
    };

private:
    main_interface *_p_main;
    u32 _state;
    list<event_c> _ev_q;
    thread _thread;
    mutex _mtx;
    int _exit_flag;
    int _ttyfd;
    fd_set _read_fd;
    string _at_recv;
    int _at_recv_state;
    int _resp_id;
    int _pid;
};

#endif
