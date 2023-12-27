#ifndef __AUTH_HANDLER_H__
#define __AUTH_HANDLER_H__

#include "common.h"
#include "main-interface.h"
#include "curl-handler.h"
#include "auth-json.h"
#include "cert-manager.h"

class auth_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    auth_handler();
    ~auth_handler();
    int init(main_interface *p_main, u32 resolution = 100);
    int deinit(void);

private:
    int auth_retry(int retry);
    int auth_state(void);
    int auth_event(void);
    int auth_proc(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

private:
    int ca_request(void);
    int enroll_request(void);
    int re_enroll_request(void);
    int token_request(void);

private:
    int auth_last_connection_record(string event_id, int reason);
    int auth_err_report(string &time, string &event_id, string &reason);
    int auth_clear(void);

public:
    enum
    {
        AUTH_START          = 0,
        AUTH_END          = 8,
    };

private:
    typedef enum
    {
        RETRY_NONE          = 0,
        STANDBY_EXECUTE     = 4
    } retry_e;

private:
    main_interface *_p_main;
    curl_handler _curl_handle;
    auth_json _json_mgr;
    cert_manager _cert_mgr;
    u32 _resolution;
    u32 _state;
    retry_e _retry_flag;
    u32 _retry_cnt;
    u32 _net_change;
    list<event_c> _ev_q;
    thread _thread;
    mutex _mtx;
    int _exit_flag;
    int _new_token_flag;
};

#endif
