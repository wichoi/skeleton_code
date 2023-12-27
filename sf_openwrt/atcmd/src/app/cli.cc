#include <string.h>

#include <iostream>
#include <list>

#include "log.h"
#include "event.h"
#include "cli.h"

using namespace chrono;

cli::cli() :
    _thread(),
    _p_main(),
    _cli_menu(),
    _resolution(10),
    _exit_flag(0)

{
    log_d("%s\n", __func__);
}

cli::~cli()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int cli::init(main_interface *p_if, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("cli::%s\n", __func__);

    _p_main = p_if;

    _cli_menu.clear();
    add_cli(&cli::help, "help", "help");
    add_cli(&cli::help, "ls", "help");
    add_cli(&cli::init, "init", "init");
    add_cli(&cli::deinit, "deinit", "deinit");

    // atcmd
    add_cli(&cli::help, "=", "=============== atcmd ===============");
    add_cli(&cli::atcmd_send, "atcmd", "AT Command Send");

    // hello world
    add_cli(&cli::help, "=", "=============== ==== ===============");
    add_cli(&cli::helloworld, "hello", "<param_1> <param_2>");

    // timer
    add_cli(&cli::set_timer, "set-timer", "<u32 timer id> <u32 interval ms>");
    add_cli(&cli::kill_timer, "kill-timer", "<u32 timer id> ");

    // config
    add_cli(&cli::conf_read, "conf-read", "nap config read");
    add_cli(&cli::conf_write, "conf-write", "nap config write");

    // exit
    add_cli(&cli::exit, "exit", "exit");

    _resolution = resolution;
    _thread = thread([&](){proc();});
    _thread.detach();

    return ret_val;
}

int cli::deinit(void)
{
    int ret_val = RET_OK;
    log_d("cli::%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _cli_menu.clear();
    return ret_val;
}

int cli::proc(void)
{
    int ret_val = RET_OK;
    log_i("cli::%s\n", __func__);

    while(_exit_flag == 0)
    {
        string read_str = "";
        getline(cin, read_str);

        if(read_str.length() > 0)
        {
            list<data_c>::iterator iter;
            for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
            {
                if(strncmp(read_str.c_str(), iter->_str.c_str(), iter->_str.length()) == 0)
                {
                    log_i("%s : %s\n", __func__, iter->_str.c_str());
                    list<string> param;
                    parser(read_str, &param);
                    fp func = iter->_func;
                    static cli *cli_func = this;
                    ret_val = (cli_func->*func)(&param);
                    break;
                }
            }
        }
        this_thread::sleep_for(milliseconds(_resolution));
    }

    return ret_val;
}

int cli::add_cli(fp func, string str, string help)
{
    int ret_val = RET_OK;
    log_v("%s %s\n", __func__, str.c_str());

    data_c data(func, str,  help);
    _cli_menu.push_back(data);

    return ret_val;
}

int cli::event_publish(u32 cmd, u32 op_code, shared_ptr<dat_c> data)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    return ret_val;
}

int cli::parser(string read_str, list<string> *param)
{
    int ret = RET_OK;
    char *ptr = NULL;
    char *next_ptr = NULL;
    log_d("%s : %s \n", __func__, read_str.c_str());

    ptr = strtok_r((char*)read_str.c_str(), " ", &next_ptr);
    while(ptr != NULL)
    {
        char buf[128] = {0,};
        memcpy(buf, ptr, strlen(ptr));
        //log_d("%s \n", buf);
        param->push_back(buf);
        ptr = strtok_r(NULL, " ", &next_ptr);
    }

    return ret;
}

int cli::help(list<string> *param)
{
    int ret_val = RET_OK;
    printf("===============  cli menu  =============== \n");
    list<data_c>::iterator iter;
    for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
    {
        printf("%s : %s\n", iter->_str.c_str(), iter->_help.c_str());
    }
    printf("========================================== \n");
    return ret_val;
}

int cli::init(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_INIT);
    return ret_val;
}

int cli::deinit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_DEINIT);
    return ret_val;
}

int cli::on_timer(u32 id)
{
    log_i("%s timer id[%u]\n", __func__, id);
    return RET_OK;
}

int cli::atcmd_send(list<string> *param)
{
    int ret_val = RET_OK;
    shared_ptr<dat_atcmd> data = make_shared<dat_atcmd>();
    data->atcmd = "atcmd_send";
    list<string>::iterator it;
    for(it = param->begin(); it != param->end(); ++it)
    {
        if(it != param->begin())
        {
            data->stream_data.append(it->c_str());
            if(it != param->end())
            {
                data->stream_data.append(" ");
            }
        }
    }
    data->stream_data.append("\r\n");
#if 0
    data->atcmd = "AT+SENDTEST";
    //data->stream_data = "AT+SENDTEST=test, 123, abc, 789, \"test\", hello \r\n";
    data->stream_data = "ATI\r\n";

    dat_atcmd::at_param_c param1(123);
    dat_atcmd::at_param_c param2("test");
    data->param.push_back(param1);
    data->param.push_back(param2);
#endif
    _p_main->event_publish(event_c::CMD_AT_TX, event_c::OP_NONE, data);
    return ret_val;
}

int cli::helloworld(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    shared_ptr<dat_hello> data = make_shared<dat_hello>();
    list<string>::iterator it;
    for(it = param->begin(); it != param->end(); ++it)
    {
        //log_d("%s\n", it->c_str());
        data->data.append(it->c_str());
        data->data.append(" ");
    }
    _p_main->event_publish(event_c::CMD_HELLOWORLD, event_c::OP_NONE, data);
    return ret_val;
}

int cli::set_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    if(param->size() == 3)
    {
        list<string>::iterator it;
        u32 t_id = 0;
        u32 t_interval = 0;
        // 0 cli menu name
        it = param->begin();

        // 1 timer id
        ++it;
        u32 id = atoi(it->c_str());

        // 2 timer interval
        ++it;
        u32 interval = atoi(it->c_str());

        _p_main->set_timer(id, interval, this);
    }
    else
    {
        log_d("%s invalid param \n", __func__);
    }
    return ret_val;
}

int cli::kill_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    if(param->size() == 2)
    {
        list<string>::iterator it;

        // 0 cli menu name
        it = param->begin();

        // 1 timer id
        ++it;
        u32 id = atoi(it->c_str());

        _p_main->kill_timer(id);
    }
    else
    {
        log_d("%s invalid param \n", __func__);
    }
    return ret_val;
}

int cli::conf_read(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_READ_CONFIG);
    return ret_val;
}

int cli::conf_write(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_WRITE_CONFIG);
    return ret_val;
}

int cli::exit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_EXIT);
    return ret_val;
}

