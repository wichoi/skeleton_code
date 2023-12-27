#include "common.h"
#include "log.h"
#include "event.h"
#include "timer.h"
#include "config-manager.h"
#include "server-proc.h"

server_proc::server_proc() :
    _event_q(),
    _timer(),
    _cli(),
    _handle(),
    _func_list()
{
    log_d("%s\n", __func__);
}

server_proc::~server_proc()
{
    log_d("%s\n", __func__);
}

int server_proc::init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);

    _func_list.clear();
    //add_func(event_c::CMD_INIT, &server_proc::init, "init");
    add_func(event_c::CMD_DEINIT, &server_proc::deinit, "deinit");

    add_func(event_c::CMD_SERVER_START, &server_proc::start_server, "start_server");
    add_func(event_c::CMD_SERVER_STOP, &server_proc::stop_server, "stop_server");

    add_func(event_c::CMD_HELLOWORLD, &server_proc::hello_world, "hello_world");

    add_func(event_c::CMD_EXIT, &server_proc::exit, "exit");

    _cli.init(this);
    _handle.init();
    _event_q.init();
    _timer.init();

    _handle.start_server(config_manager::instance()->get_grpc_url());

    return ret_val;
}

int server_proc::deinit(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _event_q.deinit();
    _timer.deinit();
    _handle.deinit();
    _cli.deinit();
    _func_list.clear();
    return ret_val;
}

int server_proc::start_server(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _handle.start_server(config_manager::instance()->get_grpc_url());
    return ret_val;
}

int server_proc::stop_server(const event_c &ev)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _handle.stop_server();
    return ret_val;
}

int server_proc::hello_world(const event_c &ev)
{
    int ret_val = RET_OK;
    shared_ptr<dat_string> data = static_pointer_cast<dat_string>(ev._data);
    log_d("%s %s\n", __func__, data->data.c_str());
    return ret_val;
}

int server_proc::exit(const event_c &ev)
{
    int ret_val = RET_EXIT;
    log_d("%s\n", __func__);
    deinit(ev);
    return ret_val;
}

int server_proc::event_publish(u32 cmd, u32 op_code, shared_ptr<dat_c> data)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _event_q.put_event(cmd, op_code, data);
    return ret_val;
}

int server_proc::proc(void)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);
    _cli.proc();
    ret_val = event_proc();
    return ret_val;
}

int server_proc::event_proc(void)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);

    event_c ev;
    if(_event_q.get_event(ev) == RET_OK)
    {
        std::list<func_list_c>::iterator iter;
        for(iter = _func_list.begin(); iter != _func_list.end(); ++iter)
        {
            if(ev._cmd == iter->_cmd)
            {
                log_v("%s\n", iter->_str.c_str());
                fp func = iter->_func;
                static server_proc *exe_func = this;
                ret_val = (exe_func->*func)(ev);
                break;
            }
        }
    }

    return ret_val;
}

int server_proc::add_func(int cmd, fp func, std::string str)
{
    int ret_val = RET_OK;
    log_v("%s %d %s\n", __func__, cmd, str.c_str());

    func_list_c data(cmd, func, str);
    _func_list.push_back(data);

    return ret_val;
}

