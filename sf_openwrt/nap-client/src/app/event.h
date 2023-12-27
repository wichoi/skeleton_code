#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include <list>
#include <mutex>

#include "common.h"
#include "event-data.h"

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
        CMD_DHCP_RENEWAL        = 4,

        // config
        CMD_READ_CONFIG         = 10,
        CMD_WRITE_CONFIG        = 11,
        CMD_UPDATE_CONFIG       = 12,

        // reboot
        CMD_REBOOT              = 20,
        CMD_FACTORY_RESET       = 21,
        CMD_CONFIG_RESET        = 22,
        CMD_RESTART             = 23,

        // test cmd
        CMD_HELLOWORLD          = 100,
        CMD_EXIT                = 101,

        // timer cmd
        CMD_TIMER_SET           = 1001,
        CMD_TIMER_KILL          = 1002,
        CMD_TIMER_PRINT         = 1003,

        // auth cmd
        CMD_AUTH_EXPIRED        = 2001,
        CMD_AUTH_DENIED         = 2002,
        CMD_AUTH_UNAUTH         = 2003,

        // at cmd
        CMD_AT_TX               = 5001,
        CMD_AT_RX               = 5002,
        CMD_AT_GPS_TX           = 5003,
        CMD_AT_GPS_RX           = 5004,

        // monitor cmd
        CMD_MON_EVENT           = 7001,
        CMD_MON_NET_CHECK       = 7002,
        CMD_MON_IP_CHANGE       = 7003,
        CMD_MON_LINK_CHANGE     = 7004,
        CMD_MON_LINK_UP         = 7005,
        CMD_MON_MEM_CHECK       = 7006,
        CMD_MON_MEM_EXCEED      = 7007,

        // watchdog ipc
        CMD_WATCH_HELLO         = 8001,
        CMD_WATCH_HELLO_ACK     = 8002,
        CMD_WATCH_SYSTEM        = 8003,
        CMD_WATCH_SYSTEM_ACK    = 8004,
        CMD_WATCH_POPEN         = 8005,
        CMD_WATCH_POPEN_ACK     = 8006,

        // server cmd
        CMD_SERVER_START        = 53001,
        CMD_SERVER_STOP         = 53002,
    };

    enum // operation code
    {
        OP_NONE                 = 0,
        OP_REQUEST              = 1,
        OP_RESPONSE             = 2,
        OP_NOTIFY               = 3,
    };

    u32 _cmd;
    u32 _op_code;
    shared_ptr<dat_c> _data;
};

class event_listener
{
public:
    event_listener() {}
    virtual ~event_listener() {}

public:
    virtual int on_event(const event_c &ev) = 0;
};

class event_queue : public event_listener
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
    event_queue();
    ~event_queue();

private:
    int on_event(const event_c &ev) {return RET_OK;};

public:
    int init(void);
    int deinit(void);
    int subscribe_event(int cmd, event_listener *p_listener);
    int put_event(u32 cmd, u32 op_code, shared_ptr<dat_c> data = NULL);
    int get_event(event_c &ev);
    int event_proc(void);

public:
    enum
    {
        QUE_MAX = 128,
    };

private:
    list<event_list> _evt_list;
    list<event_c> _ev_q;
    mutex _mtx;
    int _init_flag;
};

#endif
