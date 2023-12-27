#ifndef UI_INTERFACE_H
#define UI_INTERFACE_H

#include "main_interface.h"

class ui_interface :
    public event_listener,
    public timer_listener
{
public:
    ui_interface() {}
    virtual ~ui_interface() {}

public:
    virtual int ui_start(void) = 0;
    virtual int init(main_interface *p_main) = 0;
    virtual int deinit(void) = 0;
};

#endif // UI_INTERFACE_H
