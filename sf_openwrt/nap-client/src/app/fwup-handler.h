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
    int fwup_proc(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

    int fwup_clear(void);
    int fwup_download(void);
    int fwup_verify(void);
    int fwup_external_mcu(void);
    int fwup_nordic(void);
    int fwup_write(void);
    int fwup_sysupgrade(void);

public:
    enum
    {
        FW_START            = 0,
        FW_CHECK            = 1,
        FW_DOWNLOAD         = 2,
        FW_VERIFY           = 3,
        FW_EXTERNAL_MCU     = 4,
        FW_NORDIC           = 6,
        FW_WRITE            = 7,
        FW_REBOOT           = 8,
        FW_END              = 9
    };

    enum
    {
        FW_RESULT_NONE      = 0,
        FW_RESULT_OK        = 1,
        FW_RESULT_ERROR     = 2,
    };

private:
    main_interface *_p_main;
    curl_handler _curl_handle;
    u32 _resolution;
    u32 _state;
    list<string> _fw_list;
    //list<event_c> _ev_q;
    thread _thread;
    //mutex _mtx;
    int _exit_flag;
    atomic<int> _nordic_flag;
    string _nordic_version;
};

#endif
