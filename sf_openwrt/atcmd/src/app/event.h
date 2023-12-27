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

        // config
        CMD_READ_CONFIG         = 10,
        CMD_WRITE_CONFIG        = 11,
        CMD_UPDATE_CONFIG       = 12,

        // test cmd
        CMD_HELLOWORLD          = 100,
        CMD_EXIT                = 101,

        // timer cmd
        CMD_TIMER_SET           = 1001,
        CMD_TIMER_KILL          = 1002,

        // at cmd
        CMD_AT_TX               = 5001, // cli
        CMD_AT_RX               = 5002,
        CMD_AT_CON_TX           = 5003, // console
        CMD_AT_CON_RX           = 5004,
        CMD_AT_WEB_TX           = 5005, // web
        CMD_AT_WEB_RX           = 5006,
        CMD_AT_WEB_CMD          = 5007,
        CMD_AT_WEB_RESP         = 5008,
        CMD_AT_MODEM_TX         = 5009, // modem
        CMD_AT_MODEM_RX         = 5010,
        CMD_AT_MODEM_TIMEOUT    = 5011,
        CMD_AT_GPS_TX           = 5012, // gps
        CMD_AT_GPS_RX           = 5013,
        CMD_AT_GPS_TIMEOUT      = 5014,
        CMD_AT_UPDATE_CMD       = 5015,
        CMD_AT_UPDATE_RESP      = 5016,

        // monitor cmd
        CMD_MON_IP              = 6001,

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
