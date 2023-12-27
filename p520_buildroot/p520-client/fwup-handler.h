#ifndef __FWUP_HANDLER_H__
#define __FWUP_HANDLER_H__

#include "common.h"
#include "main-interface.h"
#include "curl-handler.h"

class fwup_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    fwup_handler();
    ~fwup_handler();
    int init(main_interface *p_main, u32 resolution = 100);
    int deinit(void);

private:
    int fwup_retry(int retry);
    int fwup_state(void);
    int fwup_event(void);

public:
    int fwup_proc(void);

private:
    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

    int fwup_clear(u32 sleep_flag = 0);
    int fwup_version_notify(void);
    int fwup_ota_start(void);
    int fwup_ota_url(void);
    int fwup_mdm_version(string &mdm_ver);
    int fwup_ota_metadata(void);
    int fwup_download(void);
    int fwup_verify(void);
    int fwup_image_exist(char *mcu_type);
    int fwup_modem(void);
    int fwup_router(void);
    int fwup_upload(void);

public:
    enum
    {
        FW_START            = 0,
        FW_VERSION_NOTI     = 1,
        FW_SMS_WAIT         = 2,
        FW_URL_REQUEST      = 3,
        FW_METADATA         = 4,
        FW_DOWNLOAD         = 5,
        FW_VERIFY           = 6,
        FW_IMAGE_CHECK      = 7,
        FW_MODEM            = 8,
        FW_ROUTER           = 9,
        FW_UPLOAD           = 10,
        FW_REBOOT           = 11,
        FW_END              = 12
    };

private:
    main_interface *_p_main;
    curl_handler _curl_handle;
    json_manager _json_handle;
    u32 _resolution;
    u32 _state;
    list<string> _fw_list;
    pthread_t _thread;
    int _exit_flag;
    int _retry_cnt;

    // ota url
    string _ota_type;
    string _ota_url;
    string _ota_timestamp;

    // fw url
    string _fw_url;
};

#endif
