#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui_interface.h"

class win_list_c
{
public:
    typedef enum
    {
        WIN_NONE            = 0,
        WIN_IDLE            = 1,
        WIN_CHARGING        = 2,
        WIN_MAX             = 3

    } win_id_e;

public:
    win_list_c(win_id_e id, shared_ptr<ui_interface> handle)
    {
        win_id = id;
        p_handle = handle;
    };

public:
    win_id_e win_id;
    shared_ptr<ui_interface> p_handle;
};

class main_window :
    public ui_interface
{
public:
    main_window(void);
    ~main_window();

public:
    // ui_interface
    int ui_start(void);
    int init(main_interface *p_main);
    int deinit(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

public:
    int selftest(void);

private:

    main_interface *_p_main;
    list<win_list_c> _win_list;
};

#endif // MAIN_WINDOW_H
