#include <sstream>
#include <iostream>

#include "log.h"
#include "utils.h"
#include "config-manager.h"
#include "monitor.h"

monitor::monitor() :
    _p_main(),
    _prev_ip(),
    _prev_link(),
    _prev_if(),
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
    _p_main->event_subscribe(event_c::CMD_MONITOR_SYSTEM, this);
    _p_main->event_subscribe(event_c::CMD_MONITOR_NETWORK, this);
    _p_main->event_subscribe(event_c::CMD_MONITOR_MEM, this);
    _p_main->event_subscribe(event_c::CMD_MONITOR_OTA, this);

    utils::read_ip_addr(_prev_ip);
    utils::read_link_state(_prev_link);
    utils::read_interface_state(_prev_if);
    _ip_null_cnt = 0;

    u32 mon_msec = config_manager::instance()->get_cloud_sysinfo_time();
    _p_main->set_timer(timer::TID_MONITOR_SYS, mon_msec, this);
    _p_main->set_timer(timer::TID_MONITOR_NET, 1000 * 5, this); // 5sec
    //_p_main->set_timer(timer::TID_MONITOR_MEM, 1000 * 60 * 60, this); // 1hour
    _p_main->set_timer(timer::TID_MONITOR_OTA, 1000 * 10, this); // 10 sec

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
    case event_c::CMD_MONITOR_SYSTEM:
        sys_info();
        break;
    case event_c::CMD_MONITOR_NETWORK:
        ip_change();
        link_change();
        break;
    case event_c::CMD_MONITOR_MEM:
        mem_check();
        break;
    case event_c::CMD_MONITOR_OTA:
        ota_reset();
        ota_sms();
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
    case timer::TID_MONITOR_SYS:
        _p_main->event_publish(event_c::CMD_MONITOR_SYSTEM);
        break;
    case timer::TID_MONITOR_NET:
        _p_main->event_publish(event_c::CMD_MONITOR_NETWORK);
        break;
    case timer::TID_MONITOR_MEM:
        _p_main->event_publish(event_c::CMD_MONITOR_MEM);
        break;
    case timer::TID_MONITOR_OTA:
        _p_main->event_publish(event_c::CMD_MONITOR_OTA);
        break;
    default:
        break;
    }
    return RET_OK;
}

int monitor::sys_info(void)
{
    string mem("");
    string cpu("");
    string ipaddr("");
    u32 uptime = utils::read_uptime();
    int event_cnt = config_manager::instance()->get_event_queue_cnt();
    int timer_cnt = config_manager::instance()->get_timer_queue_cnt();
    int cloud_cnt = config_manager::instance()->get_cloud_queue_cnt();
    utils::read_mem_usage(mem);
    utils::read_cpu_usage(cpu);
    utils::read_ip_addr(ipaddr);

    log_i("uptime[%u] ip[%s] mem[%s] cpu[%s] ev_q[%d] tm_q[%d] cl_q[%d]\n",
        uptime, ipaddr.c_str(), mem.c_str(), cpu.c_str(),
        event_cnt, timer_cnt, cloud_cnt);
    return RET_OK;
}

int monitor::mem_check(void)
{
    string mem_str = "";
    int mem = utils::read_mem_usage(mem_str);
    log_v("%s mem[%s]\n", __func__, mem_str.c_str());
    if(mem > 90)
    {
        log_w("%s mem[%d] !!! memory exceed !!!\n", __func__, mem);
        _p_main->event_publish(event_c::CMD_REBOOT);
    }

    return RET_OK;
}

int monitor::ip_change(void)
{
    string ip_addr = "";
    utils::read_ip_addr(ip_addr);
    if(ip_addr.length() > 0)
    {
        if(ip_addr.compare(_prev_ip) != 0 && (_prev_ip.length() > 0))
        {
            log_i("prev_ip[%s] ==> curr_ip[%s]\n",
                            _prev_ip.c_str(), ip_addr.c_str());
            _p_main->event_publish(event_c::CMD_NOTIFY_IP_CHANGE);
        }
        _prev_ip = ip_addr;
        _ip_null_cnt = 0;
    }
    else
    {
        if(_ip_null_cnt++ == 3)
        {
            log_i("prev_ip[%s] ==> curr_ip[null]\n", _prev_ip.c_str());
            _p_main->event_publish(event_c::CMD_NOTIFY_NER_ERROR);
        }
    }

    return RET_OK;
}

int monitor::link_change(void)
{
    bool link_st = false;
    bool if_st = false;
    utils::read_link_state(link_st);
    utils::read_interface_state(if_st);
    log_d("link_st[%d] if_st[%d]\n", link_st, if_st);

    if(_prev_link != link_st || _prev_if != if_st)
    {
        if(link_st == false || if_st == false)
        {
            _p_main->event_publish(event_c::CMD_NOTIFY_LINK_CHANGE);
        }
        else if(link_st == true || if_st == true)
        {
            _p_main->event_publish(event_c::CMD_NOTIFY_LINK_UP);
        }
    }

    _prev_link = link_st;
    _prev_if = if_st;
    return RET_OK;
}

int monitor::ota_reset(void)
{
    int ret_val = RET_OK;
    int sfota = 0;
    FILE *fp = NULL;
    log_d("%s\n", __func__);

    // URC - $$SFOTA:"devreset"
    fp = fopen("/tmp/ota_reset", "r");
    if(fp)
    {
        fscanf(fp, "%d", &sfota);
        fclose(fp);
    }

    if(sfota == 1)
    {
        _p_main->kill_timer(timer::TID_MONITOR_OTA);
        string reboot_reason = "RemoteReset";
        config_manager::instance()->set_cloud_reboot_reason(reboot_reason);
        _p_main->event_publish(event_c::CMD_REBOOT);
    }

    return ret_val;
}

int monitor::ota_sms(void)
{
    int ret_val = RET_OK;
    int sms_ota = 0;
    FILE *fp = NULL;
    log_d("%s\n", __func__);

    // URC - $$SFOTA:"devreset"
    fp = fopen("/tmp/ota_sms", "r");
    if(fp)
    {
        fscanf(fp, "%d", &sms_ota);
        fclose(fp);
        remove("/tmp/ota_sms");
    }

    if(sms_ota == 1)
    {
        int mdm_reset = 0;
        int mdm_alive = 0;
        utils::modem_sms_command(mdm_reset, mdm_alive);

        if(mdm_alive == 1)
        {
            log_i("%s sms alive detected!!!\n", __func__);
            _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_SMS_ALIVE);
        }

        if(mdm_reset == 1)
        {
            log_i("%s sms reboot detected!!!\n", __func__);
            _p_main->kill_timer(timer::TID_MONITOR_OTA);
            string reboot_reason = "RemoteReset";
            config_manager::instance()->set_cloud_reboot_reason(reboot_reason);
            _p_main->event_publish(event_c::CMD_REBOOT);
        }
    }

    return ret_val;
}

