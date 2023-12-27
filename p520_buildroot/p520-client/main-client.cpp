#include <unistd.h>

#include "log.h"
#include "utils.h"
#include "config-manager.h"
#include "main-client.h"

main_client::main_client() :
    _main_state(ST_START),
    _main_time_check_cnt(0),
    _event_handle(),
    _timer_handle(),
    _cli_handle(),
    _conf_handle(),
    _fwup_handle(),
    _cloud_handle(),
    _mon_handle()
{
    log_d("%s\n", __func__);
}

main_client::~main_client()
{
    log_d("%s\n", __func__);
}

int main_client::init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _event_handle.init();
    //_event_handle.add_event(event_c::CMD_INIT, this);
    _event_handle.subscribe_event(event_c::CMD_DEINIT, this);

    _event_handle.subscribe_event(event_c::CMD_READ_CONFIG, this);
    _event_handle.subscribe_event(event_c::CMD_WRITE_CONFIG, this);
    _event_handle.subscribe_event(event_c::CMD_UPDATE_CONFIG, this);

    _event_handle.subscribe_event(event_c::CMD_REBOOT, this);
    _event_handle.subscribe_event(event_c::CMD_FACTORY_RESET, this);
    _event_handle.subscribe_event(event_c::CMD_CONFIG_RESET, this);
    _event_handle.subscribe_event(event_c::CMD_RESTART, this);
    _event_handle.subscribe_event(event_c::CMD_MODEM_RESET, this);

    _event_handle.subscribe_event(event_c::CMD_SYSTEM_CALL, this);

    _event_handle.subscribe_event(event_c::CMD_HELLOWORLD, this);
    _event_handle.subscribe_event(event_c::CMD_EXIT, this);

    _event_handle.subscribe_event(event_c::CMD_TIMER_SET, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_KILL, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_PRINT, this);

    _event_handle.subscribe_event(event_c::CMD_MONITOR_SYSTEM, this);

    _event_handle.subscribe_event(event_c::CMD_NOTIFY_NER_ERROR, this);
    _event_handle.subscribe_event(event_c::CMD_NOTIFY_IP_CHANGE, this);
    _event_handle.subscribe_event(event_c::CMD_NOTIFY_LINK_CHANGE, this);
    _event_handle.subscribe_event(event_c::CMD_NOTIFY_LINK_UP, this);

    _conf_handle.init();
    _timer_handle.init(10);
    _cli_handle.init(this, 100);
    _fwup_handle.init(this, 100);
    _cloud_handle.init(this, 100);
    _mon_handle.init(this);
    return ret_val;
}

int main_client::deinit(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _conf_handle.deinit();
    _event_handle.deinit();
    _timer_handle.deinit();
    _cli_handle.deinit();
    _fwup_handle.deinit();
    _cloud_handle.deinit();
    _mon_handle.deinit();
    return ret_val;
}

int main_client::main_proc(void)
{
    int ret_val = RET_OK;
    //log_v("%s\n", __func__);
    return ret_val;
}

int main_client::proc(void)
{
    int ret_val = RET_OK;
    //log_v("%s\n", __func__);

    switch(_main_state)
    {
    case ST_START:
        if(utils::read_uptime() > 90)
        {
            _main_state = ST_TIME_CHECK;
        }
        else
        {
            usleep(1000 * 1000);
        }
        break;
    case ST_TIME_CHECK:
        if(utils::update_timestamp() == RET_OK)
        {
            _main_state = ST_INIT;
        }
        else
        {
            if(++_main_time_check_cnt > 3)
            {
                _main_state = ST_INIT;
            }
            else
            {
                usleep(1000 * 1000);
            }
        }
        break;
    case ST_INIT:
        if(init() == RET_OK)
        {
            _main_state = ST_IDLE;
        }
        break;
    case ST_IDLE:
        main_proc();
        ret_val = _event_handle.event_proc();
        break;
    case ST_WAIT:
        break;
    case ST_EXIT:
        break;
    default:
        log_w("%s invalid state [%d]\n", __func__, _main_state);
        break;
    }

    return ret_val;
}

int main_client::event_subscribe(u32 cmd, event_listener *p_listener)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);
    _event_handle.subscribe_event(cmd, p_listener);
    return ret_val;
}

int main_client::event_publish(u32 cmd, u32 op_code, ev_data *obj)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);
    _event_handle.put_event(cmd, op_code, obj);
    return ret_val;
}

int main_client::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_v("main_client::%s cmd[%d]\n", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_DEINIT:
        deinit(ev);
        break;
    case event_c::CMD_READ_CONFIG:
        conf_read(ev);
        break;
    case event_c::CMD_WRITE_CONFIG:
        conf_write(ev);
        break;
    case event_c::CMD_UPDATE_CONFIG:
        conf_update(ev);
        break;
    case event_c::CMD_REBOOT:
        conf_write(ev);
        main_reboot(ev);
        break;
    case event_c::CMD_FACTORY_RESET:
        main_factory_reset(ev);
        break;
    case event_c::CMD_CONFIG_RESET:
        main_config_reset(ev);
        break;
    case event_c::CMD_MODEM_RESET:
        main_modem_reset(ev);
        break;
    case event_c::CMD_SYSTEM_CALL:
        system_call(ev);
        break;
    case event_c::CMD_HELLOWORLD:
        hello_world(ev);
        break;
    case event_c::CMD_EXIT:
        ret_val = exit(ev);
        break;
    case event_c::CMD_TIMER_SET:
        {
            data_timer *obj = reinterpret_cast<data_timer*>(ev._data.GetPtr());
            _timer_handle.set_timer(obj->id, obj->interval, obj->p_timer);
        }
        break;
    case event_c::CMD_TIMER_KILL:
        {
            data_timer *obj = reinterpret_cast<data_timer*>(ev._data.GetPtr());
            _timer_handle.kill_timer(obj->id);
        }
        break;
    case event_c::CMD_TIMER_PRINT:
        print_timer(ev);
        break;
    case event_c::CMD_MONITOR_SYSTEM:
        update_sysinfo(ev);
        break;
    case event_c::CMD_NOTIFY_NER_ERROR:
        notify_network_error(ev);
        break;
    case event_c::CMD_NOTIFY_IP_CHANGE:
        notify_ip_change(ev);
        break;
    case event_c::CMD_NOTIFY_LINK_CHANGE:
        notify_link_change(ev);
        break;
    case event_c::CMD_NOTIFY_LINK_UP:
        notify_link_up(ev);
        break;
    default:
        log_v("%s should execute cmd[%d]\n", __func__, ev._cmd);
        break;
    }

    return ret_val;
}

int main_client::set_timer(u32 id, u32 interval_ms, timer_listener *p_timer)
{
    int ret_val = RET_OK;
    log_v("%s id[%u] interval[%u]\n", __func__, id, interval_ms);
    CMSSmartPtrT<data_timer> data(new data_timer, false);
    data->id = id;
    data->interval = interval_ms;
    data->p_timer = p_timer;
    event_publish(event_c::CMD_TIMER_SET, event_c::OP_NONE, data);
    return ret_val;
}

int main_client::kill_timer(u32 id)
{
    int ret_val = RET_OK;
    log_v("%s id[%u]\n", __func__,  id);
    CMSSmartPtrT<data_timer> data(new data_timer, false);
    data->id = id;
    event_publish(event_c::CMD_TIMER_KILL, event_c::OP_NONE, data);
    return ret_val;
}

int main_client::on_timer(u32 id)
{
    int ret_val = RET_OK;
    log_v("%s id[%u] \n", __func__, id);

    switch(id)
    {
    case timer::TID_CONFIG_UPDATE:
        {
            kill_timer(timer::TID_CONFIG_UPDATE);
            event_publish(event_c::CMD_WRITE_CONFIG);
        }
        break;
    case timer::TID_REBOOT:
        {
            kill_timer(timer::TID_REBOOT);
            event_publish(event_c::CMD_REBOOT);
        }
        break;
    case timer::TID_EXIT:
        {
            kill_timer(timer::TID_EXIT);
            event_publish(event_c::CMD_EXIT);
        }
        break;
    }
    return ret_val;
}

int main_client::print_timer(const event_c &ev)
{
    int ret_val = RET_OK;
    _timer_handle.print_timer();
    return ret_val;
}

int main_client::conf_read(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _conf_handle.read_config();
    return ret_val;
}

int main_client::conf_write(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _conf_handle.write_config();
    _conf_handle.write_nvram();
    return ret_val;
}

int main_client::conf_update(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    set_timer(timer::TID_CONFIG_UPDATE, 1000, this); // 1 sec
    return ret_val;
}

int main_client::main_reboot(const event_c &ev)
{
    int ret_val = RET_EXIT;
    string sys_cmd = "";
    log_i("%s\n", __func__);
    sys_cmd = "reboot -d 8 &";
    utils::system_call(sys_cmd);
    sys_cmd = "riltest REBOOT > /dev/null";
    utils::system_call(sys_cmd);
    sys_cmd = "killall -q -INT ktfotad";
    utils::system_call(sys_cmd);
    set_timer(timer::TID_EXIT, 1000, this);
    return ret_val;
}

int main_client::main_factory_reset(const event_c &ev)
{
    int ret_val = RET_EXIT;
    string sys_cmd = "";
    log_i("%s\n", __func__);
    sys_cmd = "ralink_init clear 2860";
    utils::system_call(sys_cmd);
    sys_cmd = "ralink_init renew 2860 /etc_ro/Wireless/RT2860AP/RT2860_default_vlan";
    utils::system_call(sys_cmd);
    sys_cmd = "ralink_init clear oem2";
    utils::system_call(sys_cmd);
    sys_cmd = "reboot -d 8 &";
    utils::system_call(sys_cmd);
    sys_cmd = "riltest REBOOT > /dev/null";
    utils::system_call(sys_cmd);
    sys_cmd = "killall -q -INT ktfotad";
    utils::system_call(sys_cmd);
    set_timer(timer::TID_EXIT, 1000, this);
    return ret_val;
}

int main_client::main_config_reset(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_i("%s\n", __func__);
    return ret_val;
}

int main_client::main_modem_reset(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_i("%s\n", __func__);
    string atcmd = "AT+CFUN=1,1";
    string result = "";
    utils::at_command(atcmd, result);
    return ret_val;
}

int main_client::system_call(const event_c &ev)
{
    int ret_val = RET_OK;
    data_string *str_result = reinterpret_cast<data_string*>(ev._data.GetPtr());
    log_i("%s %s\n", __func__, str_result->str_val.c_str());
    utils::system_call(str_result->str_val);
    return ret_val;
}

int main_client::hello_world(const event_c &ev)
{
    int ret_val = RET_OK;
    data_string *str_result = reinterpret_cast<data_string*>(ev._data.GetPtr());
    log_i("%s %s\n", __func__, str_result->str_val.c_str());
    return ret_val;
}

int main_client::exit(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_d("%s\n", __func__);
    deinit(ev);
    return ret_val;
}

int main_client::update_sysinfo(const event_c &ev)
{
    int ret_val = RET_OK;
    int event_cnt = _event_handle.get_queue_cnt();
    int timer_cnt = _timer_handle.get_queue_cnt();
    int cloud_cnt = _cloud_handle.get_queue_cnt();
    config_manager::instance()->set_event_queue_cnt(event_cnt);
    config_manager::instance()->set_timer_queue_cnt(timer_cnt);
    config_manager::instance()->set_cloud_queue_cnt(cloud_cnt);
    return ret_val;
}

int main_client::notify_network_error(const event_c &ev)
{
    int ret_val = RET_OK;
    u32 value = config_manager::instance()->get_network_err_cnt();
    config_manager::instance()->set_network_err_cnt(++value);
    log_i("%s cnt[%d]\n", __func__, value);
    return ret_val;
}

int main_client::notify_ip_change(const event_c &ev)
{
    int ret_val = RET_OK;
    u32 value = config_manager::instance()->get_ip_change_cnt();
    ++value;
    config_manager::instance()->set_ip_change_cnt(value);
    log_i("%s cnt[%d]\n", __func__, value);
    return ret_val;
}

int main_client::notify_link_change(const event_c &ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int main_client::notify_link_up(const event_c &ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

