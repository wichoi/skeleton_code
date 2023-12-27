#ifndef __MAIN_INTERFACE_H__
#define __MAIN_INTERFACE_H__

#include "event.h"

class main_interface
{
public:
    main_interface() {}
    virtual ~main_interface() {}

public:
    virtual int event_subscribe(u32 cmd, event_listener *p_listener) {};
    virtual int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, shared_ptr<dat_c> data = NULL) {};

    virtual int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer) {};
    virtual int kill_timer(u32 id) {};
};

#endif
