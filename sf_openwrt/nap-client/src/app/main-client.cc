#include "log.h"
#include "utils.h"
#include "config-manager.h"
#include "main-client.h"

main_client::main_client() :
    _nap_state(ST_START),
    _event_handle(),
    _timer_handle(),
    _cli_handle(),
    _conf_handler(),
    _auth_handle(),
    _grpc_handle(),
    _rms_handle(),
    _fwup_handle(),
    //_at_handle(),
    _sf_handle(),
    _wd_handle(),
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

    utils::nvram_ontime_init();

    _event_handle.init();
    //_event_handle.add_event(event_c::CMD_INIT, this);
    _event_handle.subscribe_event(event_c::CMD_DEINIT, this);
    _event_handle.subscribe_event(event_c::CMD_DHCP_RENEWAL, this);

    _event_handle.subscribe_event(event_c::CMD_READ_CONFIG, this);
    _event_handle.subscribe_event(event_c::CMD_WRITE_CONFIG, this);
    _event_handle.subscribe_event(event_c::CMD_UPDATE_CONFIG, this);

    _event_handle.subscribe_event(event_c::CMD_REBOOT, this);
    _event_handle.subscribe_event(event_c::CMD_FACTORY_RESET, this);
    _event_handle.subscribe_event(event_c::CMD_CONFIG_RESET, this);
    _event_handle.subscribe_event(event_c::CMD_RESTART, this);

    _event_handle.subscribe_event(event_c::CMD_HELLOWORLD, this);
    _event_handle.subscribe_event(event_c::CMD_EXIT, this);

    // common cmd
    _event_handle.subscribe_event(event_c::CMD_TIMER_SET, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_KILL, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_PRINT, this);

    // sigfox ipc
    _event_handle.subscribe_event(event_c::CMD_SF_DOWN_ACK, this);
    _event_handle.subscribe_event(event_c::CMD_SF_CONF_ACK, this);

    // monitor cmd
    _event_handle.subscribe_event(event_c::CMD_MON_LINK_CHANGE, this);
    _event_handle.subscribe_event(event_c::CMD_MON_LINK_UP, this);
    _event_handle.subscribe_event(event_c::CMD_MON_MEM_EXCEED, this);

    _conf_handler.init();
    _timer_handle.init(10);
    _cli_handle.init(this, 100);
    _auth_handle.init(this, 100);
    _grpc_handle.init(this, 100);
    _rms_handle.init(this, 100);
    _fwup_handle.init(this, 100);
    //_at_handle.init(this);
    _sf_handle.init(this);
    _wd_handle.init(this);
    _mon_handle.init(this);

    return ret_val;
}

int main_client::deinit(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _event_handle.deinit();
    _timer_handle.deinit();
    _conf_handler.deinit();
    _auth_handle.deinit();
    _grpc_handle.deinit();
    _rms_handle.deinit();
    _fwup_handle.deinit();
    //_at_handle.deinit();
    _cli_handle.deinit();
    _sf_handle.deinit();
    _wd_handle.deinit();
    _mon_handle.deinit();
    return ret_val;
}

int main_client::nap_proc(void)
{
    int ret_val = RET_OK;
    //log_v("%s\n", __func__);
    return ret_val;
}

int main_client::proc(void)
{
    int ret_val = RET_OK;
    //log_v("%s\n", __func__);

    switch(_nap_state)
    {
    case ST_START:
        if(utils::read_uptime() > 60)
        {
            _nap_state = ST_INIT;
        }
        else
        {
            this_thread::sleep_for(milliseconds(1000));
        }
        break;
    case ST_INIT:
        if(init() == RET_OK)
        {
            _nap_state = ST_STANDBY;
        }
        break;
    case ST_STANDBY:
        //nap_proc();
        ret_val = _event_handle.event_proc();
        break;
    case ST_WAIT:
        break;
    case ST_EXIT:
        break;
    default:
        log_w("%s invalid state [%d]\n", __func__, _nap_state);
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

int main_client::event_publish(u32 cmd, u32 op_code, shared_ptr<dat_c> data)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);
    _event_handle.put_event(cmd, op_code, data);
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
    case event_c::CMD_DHCP_RENEWAL:
        utils::udhcpc_renewal();
        break;
    case event_c::CMD_READ_CONFIG:
        nap_conf_read(ev);
        break;
    case event_c::CMD_WRITE_CONFIG:
        nap_conf_write(ev);
        break;
    case event_c::CMD_UPDATE_CONFIG:
        nap_conf_update(ev);
        break;
    case event_c::CMD_REBOOT:
        nap_reboot(ev);
        break;
    case event_c::CMD_FACTORY_RESET:
        nap_factory_reset(ev);
        break;
    case event_c::CMD_CONFIG_RESET:
        nap_config_reset(ev);
        break;
    case event_c::CMD_RESTART:
        nap_conf_update(ev);
        set_timer(timer::TID_EXIT, 5000, this);
        break;
    case event_c::CMD_HELLOWORLD:
        hello_world(ev);
        break;
    case event_c::CMD_EXIT:
        exit(ev);
        break;
    case event_c::CMD_TIMER_SET:
        {
            shared_ptr<dat_timer> recv = static_pointer_cast<dat_timer>(ev._data);
            _timer_handle.set_timer(recv->id, recv->interval, recv->p_timer);
        }
        break;
    case event_c::CMD_TIMER_KILL:
        {
            shared_ptr<dat_timer> recv = static_pointer_cast<dat_timer>(ev._data);
            _timer_handle.kill_timer(recv->id);
        }
        break;
    case event_c::CMD_TIMER_PRINT:
        print_timer(ev);
        break;
    case event_c::CMD_SF_DOWN_ACK:
        {
            shared_ptr<dat_ipc_result> recv = static_pointer_cast<dat_ipc_result>(ev._data);
            _grpc_handle.grpc_sigfox_ipc_result(ev._cmd, recv->id, recv->result);
        }
        break;
    case event_c::CMD_SF_CONF_ACK:
        {
            shared_ptr<dat_ipc_result> recv = static_pointer_cast<dat_ipc_result>(ev._data);
            _grpc_handle.grpc_sigfox_ipc_result(ev._cmd, recv->id, recv->result);
        }
        break;
    case event_c::CMD_MON_LINK_UP:
        {
            set_timer(timer::TID_LINK_CHANGE, 1000, this);
        }
        break;
    case event_c::CMD_MON_MEM_EXCEED:
        {
            set_timer(timer::TID_REBOOT, 5000, this);
        }
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
    shared_ptr<dat_timer> data = make_shared<dat_timer>();
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
    shared_ptr<dat_timer> data = make_shared<dat_timer>();
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
    case timer::TID_LINK_CHANGE:
        {
            kill_timer(timer::TID_LINK_CHANGE);
            event_publish(event_c::CMD_DHCP_RENEWAL);
        }
        break;
    case timer::TID_NAP_UPDATE_CONF:
        {
            kill_timer(timer::TID_NAP_UPDATE_CONF);
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

int main_client::nap_conf_read(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _conf_handler.read_config();
    return ret_val;
}

int main_client::nap_conf_write(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _conf_handler.write_config();
    return ret_val;
}

int main_client::nap_conf_update(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    set_timer(timer::TID_NAP_UPDATE_CONF, 1000, this); // 1 sec
    return ret_val;
}

int main_client::nap_reboot(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_i("%s\n", __func__);

    shared_ptr<dat_string> data = make_shared<dat_string>();
    data->data = "reboot -d 8";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);
    //utils::system_reboot();
    set_timer(timer::TID_EXIT, 1000, this);
    return ret_val;
}

int main_client::nap_factory_reset(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_i("%s\n", __func__);

    shared_ptr<dat_string> clear = make_shared<dat_string>();
    clear->data = "nvram clear 2860";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, clear);

    shared_ptr<dat_string> renew = make_shared<dat_string>();
    renew->data = "nvram renew 2860 /etc/wireless/mediatek/mt7603/mt7603.dat.bak";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, renew);

    shared_ptr<dat_string> commit = make_shared<dat_string>();
    commit->data = "nvram commit";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, commit);

    shared_ptr<dat_string> data = make_shared<dat_string>();
    data->data = "rm -rf /data/*";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);

    shared_ptr<dat_string> overlay = make_shared<dat_string>();
    overlay->data = "rm -rf /overlay/upper /overlay/work";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, overlay);

    shared_ptr<dat_string> reboot = make_shared<dat_string>();
    reboot->data = "reboot -d 8";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, reboot);

    //utils::system_factory_reset();
    return ret_val;
}

int main_client::nap_config_reset(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_i("%s\n", __func__);

    shared_ptr<dat_string> clear = make_shared<dat_string>();
    clear->data = "nvram clear 2860";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, clear);

    shared_ptr<dat_string> renew = make_shared<dat_string>();
    renew->data = "nvram renew 2860 /etc/wireless/mediatek/mt7603/mt7603.dat.bak";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, renew);

    shared_ptr<dat_string> commit = make_shared<dat_string>();
    commit->data = "nvram commit";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, commit);

    shared_ptr<dat_string> data = make_shared<dat_string>();
    data->data = "rm -rf /data/*.conf* /data/*.log*";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);

    shared_ptr<dat_string> overlay = make_shared<dat_string>();
    overlay->data = "rm -rf /overlay/upper /overlay/work";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, overlay);

    shared_ptr<dat_string> reboot = make_shared<dat_string>();
    reboot->data = "reboot -d 8";
    event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, reboot);

    return ret_val;
}

int main_client::hello_world(const event_c &ev)
{
    int ret_val = RET_OK;
    shared_ptr<dat_string> data = static_pointer_cast<dat_string>(ev._data);
    log_i("%s %s\n", __func__, data->data.c_str());
    return ret_val;
}

int main_client::exit(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_d("%s\n", __func__);
    deinit(ev);
    return ret_val;
}

