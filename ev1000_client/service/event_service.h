#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <QMutex>

#include <string>
#include <list>
#include <memory>

#include "common.h"
#include "timer_service.h"

class ev_data {};

class data_string : public ev_data
{
public:
    string str_val;
};

class data_int : public ev_data
{
public:
    int value;
};

class data_timer : public ev_data
{
public:
    u32 id;
    u32 interval; // ms
    timer_listener *p_timer;
};

class event_c
{
public:
    enum
    {
        // system cmd
        CMD_NONE                = 0,
        CMD_HELP                = 1,
        CMD_INIT                = 2,
        CMD_DEINIT              = 3,


        // config
        CMD_READ_CONFIG         = 10,
        CMD_WRITE_CONFIG        = 11,
        CMD_UPDATE_CONFIG       = 12,

        // reboot
        CMD_REBOOT              = 20,
        CMD_FACTORY_RESET       = 21,
        CMD_CONFIG_RESET        = 22,
        CMD_RESTART             = 23,
        CMD_MODEM_RESET         = 24,

        CMD_SYSTEM_CALL         = 50,

        // test cmd
        CMD_HELLOWORLD          = 100,
        CMD_SELF_TEST           = 101,
        CMD_MEDIA_TEST          = 102,
        CMD_IDLE_TEST           = 103,
        CMD_CHARGING_TEST       = 104,

        // timer cmd
        CMD_TIMER_SET           = 1001,
        CMD_TIMER_KILL          = 1002,
        CMD_TIMER_PRINT         = 1003,

        CMD_MONITOR_SYSTEM      = 2000,
        CMD_MONITOR_NETWORK     = 2001,
        CMD_MONITOR_MEM         = 2002,

        CMD_MONITOR_OTA         = 2100,

        CMD_NOTIFY_NER_ERROR    = 3000,
        CMD_NOTIFY_IP_CHANGE    = 3001,
        CMD_NOTIFY_LINK_CHANGE  = 3002,
        CMD_NOTIFY_LINK_UP      = 3003,

        CMD_EXIT                = 8000,

        CMD_EVENT_MAX           = 9999,
    };

    enum // operation code
    {
        OP_NONE                 = 0,
        OP_REQUEST              = 1,
        OP_RESPONSE             = 2,
        OP_NOTIFY               = 3,

        OP_NER_ERR              = 1000,
        OP_IP_CHANGE            = 1001,
        OP_LAN_CHANGE           = 1002,
        OP_SMS_ALIVE            = 1003,
    };

    u32 _cmd;
    u32 _op_code;
    shared_ptr<ev_data> _data;
};

class event_listener
{
public:
    event_listener() {}
    virtual ~event_listener() {}

public:
    virtual int on_event(const event_c &ev) = 0;
};

class event_service : public event_listener
{
private:
    class event_list
    {
    public:
        int _cmd;
        event_listener *_p_interface;
        event_list(int cmd, event_listener *p_listener)
        {
            _cmd = cmd;
            _p_interface = p_listener;
        }
    };

public:
    event_service();
    ~event_service();

private:
    int on_event(const event_c &ev) {ev; return RET_OK;};

public:
    int init(void);
    int deinit(void);
    int subscribe_event(int cmd, event_listener *p_listener);
    int put_event(u32 cmd, u32 op_code, shared_ptr<ev_data> data = NULL);
    int get_event(event_c &ev);
    int get_queue_cnt(void);
    int event_proc(void);

public:
    enum
    {
        QUE_MAX = 128,
    };

private:
    list<event_list> _evt_list;
    list<event_c> _ev_q;
    QMutex _mtx;
    int _init_flag;
};

#endif // EVENT_SERVICE_H
