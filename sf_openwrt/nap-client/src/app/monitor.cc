#include <sstream>
#include <iostream>

#include "log.h"
#include "utils.h"
#include "config-manager.h"
#include "monitor.h"

#define RETRY_CNT       3

monitor::monitor() :
    _p_main(),
    _prev_ip(),
    _prev_link(),
    _prev_if(),
    _watch_cnt(),
    _gps_cnt(),
    _gps_active(),
    _ip_null_cnt()
{
    log_d("%s\n", __func__);
}

monitor::~monitor()
{
    log_d("%s\n", __func__);
}

int monitor::init(main_interface *p_main)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_MON_NET_CHECK, this);
    _p_main->event_subscribe(event_c::CMD_MON_MEM_CHECK, this);
    //_p_main->event_subscribe(event_c::CMD_WATCH_HELLO_ACK, this);

    string conn_ifname = "";
    utils::read_conn_interface(conn_ifname);
    string ip_type = "";
	bool vlan_valid = false;
    utils::read_ip_type(ip_type);
    utils::read_ip_addr(_prev_ip, ip_type, conn_ifname);

    // mii register has not initialized. so first readed value is invalid.
    utils::read_link_state(_prev_link, conn_ifname);
    utils::read_vlan_table(vlan_valid);
    utils::read_interface_state(_prev_if, conn_ifname);
    utils::read_link_state(_prev_link, conn_ifname);
    utils::read_interface_state(_prev_if, conn_ifname);

    _watch_cnt = 0;
    _gps_cnt = 0;
    _gps_active = 0;
    _ip_null_cnt = 3;

    //_p_main->set_timer(timer::TID_SYS_MONITOR, 1000 * 10, this); // 10sec
    _p_main->set_timer(timer::TID_NET_MONITOR, 1000 * 3, this); // 3sec
    //_p_main->set_timer(timer::TID_MEM_MONITOR, 1000 * 60 * 60 * 24, this); // 1day
    return ret_val;
}

int monitor::deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main = NULL;
    return ret_val;
}

int monitor::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_v("monitor::%s cmd[%d]\n", __func__, ev._cmd);

    switch(ev._cmd)
    {
    case event_c::CMD_MON_NET_CHECK:
        gps_check();
        if(_gps_cnt == 0)
        {
            ip_change();
            link_change();
            vlan_change();
        }
        break;
    case event_c::CMD_WATCH_HELLO_ACK:
        nap_watchdog(1);
        break;
    case event_c::CMD_MON_MEM_CHECK:
        mem_check();
        break;
    default:
        log_v("%s should execute cmd[%d]\n", __func__, ev._cmd);
        break;
    }

    return ret_val;
}

int monitor::on_timer(u32 id)
{
    log_v("%s id[%u] \n", __func__, id);
    switch(id)
    {
    case timer::TID_SYS_MONITOR:
        {
            _p_main->event_publish(event_c::CMD_WATCH_HELLO);
            nap_watchdog(0);
        }
        break;
    case timer::TID_NET_MONITOR:
        {
            _p_main->event_publish(event_c::CMD_MON_NET_CHECK);
        }
        break;
    case timer::TID_MEM_MONITOR:
        {
            _p_main->event_publish(event_c::CMD_MON_MEM_CHECK);
        }
        break;
    default:
        break;
    }
    return RET_OK;
}

int monitor::mem_check(void)
{
    string mem_str = "";
    int mem = utils::read_mem_usage(mem_str);
    log_v("%s mem[%s]\n", __func__, mem_str.c_str());
    if(mem > 90)
    {
        log_w("%s mem[%d] !!! memory exceed !!! \n", __func__, mem);
        _p_main->event_publish(event_c::CMD_MON_MEM_EXCEED);
    }

    return RET_OK;
}

int monitor::nap_watchdog(int kick) // kick(0), ack(1)
{
#ifdef LINUX_PC_APP
    // do nothing
#else
    if(kick == 0)
    {
        _watch_cnt++;
    }
    else
    {
        _watch_cnt = 0;
    }

    if(_watch_cnt > RETRY_CNT)
    {
        string cmd = "";
        log_i("nap-watchdog standby !!!\n");
        cmd = "killall -9 nap-watchdog";
        utils::system_call(cmd);
        cmd = "nap-watchdog &";
        utils::system_call(cmd);
        //shared_ptr<dat_string> data = make_shared<dat_string>();
        //data->data = "killall -9 nap-watchdog";
        //_p_main->event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);
        //data->data = "nap-watchdog &";
        //_p_main->event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);
        _watch_cnt = 0;
    }
#endif
    return RET_OK;
}

int monitor::ip_change(void)
{
    string conn_ifname = "";
    utils::read_conn_interface(conn_ifname);
    string ip_type = "";
    utils::read_ip_type(ip_type);
    string ip_addr = "";
    utils::read_ip_addr(ip_addr, ip_type, conn_ifname);
    if(ip_addr.length() > 0)
    {
        if(ip_addr.compare(_prev_ip) != 0 && (_prev_ip.length() > 0))
        {
            log_i("prev_ip[%s] ==> curr_ip[%s]\n", _prev_ip.c_str(), ip_addr.c_str());
            _p_main->event_publish(event_c::CMD_MON_IP_CHANGE);
        }
        _prev_ip = ip_addr;
        _ip_null_cnt = 0;
    }
    else
    {
        if(_ip_null_cnt++ == 3)
        {
            log_i("prev_ip[%s] ==> curr_ip[null]\n", _prev_ip.c_str());
            _p_main->event_publish(event_c::CMD_MON_IP_CHANGE);
        }
    }

    return RET_OK;
}

int monitor::link_change(void)
{
    bool link_st = false;
    bool if_st = false;
    string conn_ifname = "";
    utils::read_conn_interface(conn_ifname);
    utils::read_link_state(link_st, conn_ifname);
    utils::read_interface_state(if_st, conn_ifname);
    log_d("link_st[%d] if_st[%d]\n", link_st, if_st);

    if(_prev_link != link_st || _prev_if != if_st)
    {
        if(link_st == false || if_st == false)
        {
            _p_main->event_publish(event_c::CMD_MON_LINK_CHANGE);
        }
        else if(link_st == true || if_st == true)
        {
            _p_main->event_publish(event_c::CMD_MON_LINK_UP);
        }
    }

    _prev_link = link_st;
    _prev_if = if_st;
    return RET_OK;
}

int monitor::vlan_change(void)
{
    bool vlan_valid = false;
    utils::read_vlan_table(vlan_valid);

    if(vlan_valid == false)
    {
    	log_i("%s : unexpectecd VLAN table reset , rebooting...\n", __func__);
        _p_main->event_publish(event_c::CMD_REBOOT);
    }

    return RET_OK;
}

int monitor::gps_check(void)
{
    int result = 0; // 0:none, 1:gps(lte), 2gps(wan)
    if(utils::modem_gps_updating_check() == RET_OK)
    {
        if(_gps_cnt == 0)
        {
            string conn_type = "";
            utils::read_conn_type(conn_type);
            if(conn_type.compare("LTE") == 0) // BACKHAUL_LTE
            {
                _gps_active = 1;
                config_manager::instance()->set_lte_gps_status(1);
                //_p_main->event_publish(event_c::CMD_GPS_ACTIVE_LTE_DISCONNECT);
            }
            else
            {
                _gps_active = 2;
                config_manager::instance()->set_lte_gps_status(2);
            }
        }
        _gps_cnt = 20; // 3 * 20 = 60sec
    }

    if(_gps_cnt > 0)
    {
        _gps_cnt--;
    }

    if(_gps_cnt == 0)
    {
        if(_gps_active != 0)
        {
            config_manager::instance()->set_lte_gps_status(0);
            //_p_main->event_publish(event_c::CMD_GPS_INACTIVE_LTE_CONNECT);
        }
        _gps_active = 0;
    }

    return RET_OK;
}

