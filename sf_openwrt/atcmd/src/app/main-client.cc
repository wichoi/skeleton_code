#include "log.h"
#include "event.h"
#include "utils.h"
#include "config-manager.h"
#include "main-client.h"

main_client::main_client() :
    _at_state(ST_START),
    _event_handle(),
    _timer_handle(),
    _cli_handle(),
    _conf_handle(),
    _at_handle(),
    _con_handle(),
    _web_handle(),
    _modem_handle(),
    _gps_handle()
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
    _event_handle.subscribe_event(event_c::CMD_DEINIT, this);

    _event_handle.subscribe_event(event_c::CMD_READ_CONFIG, this);
    _event_handle.subscribe_event(event_c::CMD_WRITE_CONFIG, this);
    _event_handle.subscribe_event(event_c::CMD_UPDATE_CONFIG, this);

    _event_handle.subscribe_event(event_c::CMD_HELLOWORLD, this);
    _event_handle.subscribe_event(event_c::CMD_EXIT, this);

    // common cmd
    _event_handle.subscribe_event(event_c::CMD_TIMER_SET, this);
    _event_handle.subscribe_event(event_c::CMD_TIMER_KILL, this);

    _timer_handle.init(10);
    _cli_handle.init(this);
    _conf_handle.init();
    _at_handle.init(this);
    _con_handle.init(this);
    _web_handle.init(this);
    _modem_handle.init(this);
    _gps_handle.init(this);

    return ret_val;
}

int main_client::deinit(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _event_handle.deinit();
    _timer_handle.deinit();
    _at_handle.deinit();
    _cli_handle.deinit();
    _conf_handle.deinit();
    _con_handle.deinit();
    _web_handle.deinit();
    _modem_handle.deinit();
    _gps_handle.deinit();
    return ret_val;
}

int main_client::at_proc(void)
{
    int ret_val = RET_OK;
    //log_v("%s\n", __func__);
    return RET_OK;
}

int main_client::proc(void)
{
    int ret_val = RET_OK;
    //log_v("%s\n", __func__);

    switch(_at_state)
    {
    case ST_START:
        if(utils::read_uptime() > 60)
        {
            _at_state = ST_INIT;
        }
        else
        {
            this_thread::sleep_for(milliseconds(1000));
        }
        break;
    case ST_INIT:
        if(init() == RET_OK)
        {
            log_i("%s init done !!!\n", __func__);
            _at_state = ST_STANDBY;
        }
        break;
    case ST_STANDBY:
        //at_proc();
        _con_handle.ipc_proc();
        _web_handle.ipc_proc();
        _modem_handle.modem_proc();
        _gps_handle.gps_proc();
        ret_val = _event_handle.event_proc();
        break;
    case ST_WAIT:
        break;
    case ST_EXIT:
        break;
    default:
        log_w("%s invalid state [%d]\n", __func__, _at_state);
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
    log_d("main_client::%s cmd[%d]\n", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_DEINIT:
        deinit(ev);
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
    default:
        log_v("%s should execute cmd[%d]\n", __func__, ev._cmd);
        break;
    }

    return ret_val;
}

int main_client::set_timer(u32 id, u32 interval_ms, timer_listener *p_timer)
{
    int ret_val = RET_OK;
    log_d("%s id[%u] interval[%u]\n", __func__, id, interval_ms);
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
    log_d("%s id[%u]\n", __func__,  id);
    shared_ptr<dat_timer> data = make_shared<dat_timer>();
    data->id = id;
    event_publish(event_c::CMD_TIMER_KILL, event_c::OP_NONE, data);
    return ret_val;
}

int main_client::on_timer(u32 id)
{
    int ret_val = RET_OK;
    log_d("%s id[%u] \n", __func__, id);

    switch(id)
    {
    case timer::TID_NAP_UPDATE_CONF:
        {
            kill_timer(timer::TID_NAP_UPDATE_CONF);
            event_publish(event_c::CMD_WRITE_CONFIG);
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
    _conf_handle.read_config();
    return ret_val;
}

int main_client::nap_conf_write(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _conf_handle.write_config();
    return ret_val;
}

int main_client::nap_conf_update(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    set_timer(timer::TID_NAP_UPDATE_CONF, 1000, this); // 1 sec
    return ret_val;
}

int main_client::hello_world(const event_c &ev)
{
    int ret_val = RET_OK;
    shared_ptr<dat_hello> data = static_pointer_cast<dat_hello>(ev._data);
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

