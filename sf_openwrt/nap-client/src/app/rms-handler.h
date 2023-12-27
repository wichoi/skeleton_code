#ifndef __RMS_HANDLER_H__
#define __RMS_HANDLER_H__

#include "common.h"
#include "main-interface.h"
#include "curl-handler.h"
#include "rms-json.h"
#include "cert-manager.h"

class rms_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    rms_handler();
    ~rms_handler();
    int init(main_interface *p_main, u32 resolution);
    int deinit(void);

private:
    int rms_retry(int retry);
    int rms_state(void);
    int rms_event(void);
    int rms_proc(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

    int rms_boot(void);
    int rms_fw_check(void);
    int rms_fw_telemetry(string &fw_st);
    int rms_ca(void);
    int rms_white_list(void);
    int rms_error(void);
    int rms_token(void);
    int rms_polling(list<json_param> &get_list, list<json_param> &set_list, list<json_param> &bulk_list, string &ev_id);
    int rms_result(list<json_param> &get_list, list<json_param> &set_list, list<json_param> &bulk_list, string &ev_id);

private:
    int rms_result_get(list<json_param> &get_list, u32 is_bulk = 0);
    int rms_result_set(list<json_param> &set_list);
    int rms_last_connection_record(string &timestamp, string event_id, int reason);
    int rms_err_report(string &time, string &event_id, string &reason);
    int rms_error_list(const event_c &ev);

public:
    enum
    {
        RMS_START           = 0,
        RMS_RESULT          = 9,
    };

private:
    typedef enum
    {
        RETRY_NONE          = 0,
        POLLING_EXECUTE     = 4
    } retry_e;

private:
    main_interface *_p_main;
    curl_handler _curl_handle;
    rms_json _json_mgr;
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
    string _prev_url;
    list<shared_ptr<dat_rms_error>> _err_list;
    string _poll_ev_id;
    list<json_param> _get_list;
    list<json_param> _set_list;
    list<json_param> _bulk_list;
    bool _wait_syscall_response;
};

#endif
