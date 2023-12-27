#ifndef __CLOUD_HANDLER_H__
#define __CLOUD_HANDLER_H__

#include "common.h"
#include "main-interface.h"
#include "auth-handler.h"
#include "curl-handler.h"

class cloud_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    cloud_handler();
    ~cloud_handler();
    int init(main_interface *p_main, u32 resolution = 100);
    int deinit(void);

private:
    int cloud_retry(int retry);
    int cloud_state(void);
    int cloud_event(void);

public:
    int cloud_proc(void);
    int get_queue_cnt(void);

private:
    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

    int cloud_st_clear(void);

    // server api
    int cloud_auth(void);
    int cloud_sauth(void);
    int cloud_boot(void);
    int cloud_event_inform(event_c &ev);
    int cloud_report(void);
    int cloud_heartbeat(void);
    int cloud_getkey(void);
    int cloud_keyupdated(void);
    int cloud_getparam(void);
    int cloud_paramupdated(void);
    int cloud_getfwup(void);
    int cloud_fwup(void);

    int make_secure_number(string &secure_num);
    int make_secure_info(string &sinfo, string &khint);
    int make_secure_boot(string &sinfo, string &khint);
    int decrypt_secure_cmd(string &dest_cmd, string &secure_cmd);
    int decrypt_secure_key(string &cs, string &skey);
    int set_report_time(int interval);
    int set_heartbeat_time(int interval);
    int set_retry_time(int interval);
    int set_cloud_param(list<result_dev_param_t> &params);
    int execute_cloud_cmd(list<result_cmds_t> &cmds);
    int response_result(string &result);

    // fwup
    int fwup_clear(void);
    int fwup_prepare(result_getfwup_t result);
    int fwup_download(void);
    int fwup_time_calculation(void);
    int fwup_verify(void);
    int fwup_install(void);

private:
    typedef enum
    {
        RETRY_NONE          = 0,
        RETRY_SET           = 1,
        RETRY_EXECUTE       = 2,
        POLLING_SET         = 3,
        POLLING_EXECUTE     = 4,
        WAIT_GET_KEY        = 5
    } retry_e;

private:
    enum
    {
        CLD_START               = 0,
        CLD_INIT                = 1,
        CLD_NET_CHECK           = 2,
        CLD_AUTH                = 3,
        CLD_GET_KEY             = 4,
        CLD_BOOT                = 5,
        CLD_POLLING             = 6,
        CLD_END                 = 7
    };

private:
    main_interface *_p_main;
    auth_handler _auth_handle;
    curl_handler _curl_handle;
    json_manager _json_handle;
    u32 _resolution;
    u32 _state;
    retry_e _retry_flag;
    u32 _retry_cnt;
    u32 _get_key_flag; // 0(none) 1(get_key)
    u32 _param_update_flag;
    u32 _not_found_flag;
    list<event_c> _ev_q;
    pthread_t _thread;
    pthread_mutex_t _mtx;
    int _exit_flag;
};

#endif
