#ifndef __MODEM_HANDLER_H__
#define __MODEM_HANDLER_H__

#include <atomic>

#include "common.h"
#include "event.h"
#include "timer.h"
#include "main-interface.h"

class modem_handler :
    public main_interface,
    public event_listener,
    public timer_listener
{
public:
    modem_handler();
    ~modem_handler();
    int init(main_interface *p_if);
    int deinit(void);
    int modem_proc(void);

private:
    // event_listener
    int on_event(const event_c &ev);

    // timer_listener
    int on_timer(u32 id);

private:
    int at_tx(string atcmd);
    int at_rx_gsn(string &rx_str);
    int at_rx_ati(string &rx_str);
    int at_rx_qappver(string &rx_str);
    int at_rx_cimi(string &rx_str);
    int at_rx_cpin(string &rx_str);
    int at_rx_qccid(string &rx_str);
    int at_rx_cops(string &rx_str);
    int at_rx_qnwinfo(string &rx_str);
    int at_rx_qcsq(string &rx_str);
    int at_rx_qspn(string &rx_str);
    int at_rx_cgdcont(string &rx_str);
    int at_rx_cclk(string &rx_str);
    int at_rx_qinistat(string &rx_str);
    int at_rx_cnum(string &rx_str);

public:
    enum
    {
        ST_START            = 0,
        ST_INIT             = 1,
        ST_GSN_TX           = 2,
        ST_GSN_RX           = 3,
        ST_ATI_TX           = 4,
        ST_ATI_RX           = 5,
        ST_VER_TX           = 6,
        ST_VER_RX           = 7,
        ST_CIMI_TX          = 8,
        ST_CIMI_RX          = 9,
        ST_CPIN_TX          = 10,
        ST_CPIN_RX          = 11,
        ST_QCCID_TX         = 12,
        ST_QCCID_RX         = 13,
        ST_COPS_TX          = 14,
        ST_COPS_RX          = 15,
        ST_QNWINFO_TX       = 16,
        ST_QNWINFO_RX       = 17,
        ST_QCSQ_TX          = 18,
        ST_QCSQ_RX          = 19,
        ST_QSPN_TX          = 20,
        ST_QSPN_RX          = 21,
        ST_CGDCONT_TX       = 22,
        ST_CGDCONT_RX       = 23,
        ST_CCLK_TX          = 24,
        ST_CCLK_RX          = 25,
        ST_QINISTAT_TX      = 26,
        ST_QINISTAT_RX      = 27,
        ST_CNUM_TX          = 28,
        ST_CNUM_RX          = 29,
        ST_RESP             = 30,
        ST_END              = 31
    };

    enum
    {
        UPDATE_CMD_NONE     = 0,
        UPDATE_CMD_RECV     = 1,
        UPDATE_CMD_CONSOLE  = 2
    };

private:
    main_interface *_p_main;
    atomic<int> _state;
    string _rx_buffer;
    u32 _update_cmd;
};

#endif
