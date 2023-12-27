#ifndef MAIN_INTERFACE_H
#define MAIN_INTERFACE_H

#include "event_service.h"
#include "timer_service.h"

class main_interface
{
public:
    main_interface() {}
    virtual ~main_interface() {}

public:
    virtual int event_subscribe(u32 cmd, event_listener *p_listener) {cmd;p_listener;return 0;};
    virtual int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, shared_ptr<ev_data> data = NULL) {cmd;op_code;data;return 0;};

    virtual int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer) {id; interval_ms; p_timer; return 0;};
    virtual int kill_timer(u32 id) {id; return 0;};
};

#endif // MAIN_INTERFACE_H
