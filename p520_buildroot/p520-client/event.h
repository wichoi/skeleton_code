#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include <pthread.h>
#include <list>

#include "FSPObjTypes.h"

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

        // reboot
        CMD_REBOOT              = 20,
        CMD_FACTORY_RESET       = 21,
        CMD_CONFIG_RESET        = 22,
        CMD_RESTART             = 23,
        CMD_MODEM_RESET         = 24,

        CMD_SYSTEM_CALL         = 50,

        // test cmd
        CMD_HELLOWORLD          = 100,
        CMD_EXIT                = 101,

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

        // cloud control
        CMD_CLOUD_ST_CLEAR      = 4001,

        // cloud cmd
        CMD_CLOUD_AUTH          = 4100,
        CMD_CLOUD_SAUTH         = 4101,
        CMD_CLOUD_BOOT          = 4102,
        CMD_CLOUD_EVENT         = 4103,
        CMD_CLOUD_REPORT        = 4104,
        CMD_CLOUD_HEARTBEAT     = 4105,
        CMD_CLOUD_GET_KEY       = 4106,
        CMD_CLOUD_KEY_UPDATED   = 4107,
        CMD_CLOUD_GET_PARAM     = 4108,
        CMD_CLOUD_PARAM_UPDATED = 4109,
        CMD_CLOUD_GET_FWUP      = 4110,
        CMD_CLOUD_FWUP          = 4111,

        // cloud fwup
        CMD_FWUP_CLEAR          = 4121,
        CMD_FWUP_DOWNLOAD       = 4122,
        CMD_FWUP_TIME_CALC      = 4123,
        CMD_FWUP_VERIFY         = 4124,
        CMD_FWUP_INSTALL        = 4125,

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
    CMSSmartPtrT<ev_data> _data;
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
    int put_event(u32 cmd, u32 op_code, ev_data *obj = NULL);
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
    pthread_mutex_t _mtx;
    int _init_flag;
};

#endif
