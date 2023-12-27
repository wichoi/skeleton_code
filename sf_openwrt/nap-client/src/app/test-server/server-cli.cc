#include <string.h>

#include <iostream>
#include <list>

#include "common.h"
#include "log.h"
#include "event.h"
#include "timer.h"
#include "server-cli.h"

server_cli::server_cli():
    _p_main(),
    _cli_menu()
{
    log_d("%s\n", __func__);
}

server_cli::~server_cli()
{
    log_d("%s\n", __func__);
}

int server_cli::init(main_interface *p_main)
{
    int ret_val = RET_OK;
    log_d("server_cli::%s\n", __func__);

    _p_main = p_main;

    _cli_menu.clear();
    add_cli(&server_cli::cli_help, "help", "help");
    add_cli(&server_cli::cli_init, "init", "init");
    add_cli(&server_cli::cli_deinit, "deinit", "deinit");

    add_cli(&server_cli::cli_start, "start", "start grpc server");
    add_cli(&server_cli::cli_stop, "stop", "stop grpc server");

    add_cli(&server_cli::cli_helloworld, "hello", "<param_1> <param_2>");

    add_cli(&server_cli::cli_exit, "exit", "exit");

    return ret_val;
}

int server_cli::deinit(void)
{
    int ret_val = RET_OK;
    log_d("server_cli::%s\n", __func__);
    _p_main = NULL;
    _cli_menu.clear();
    return ret_val;
}

int server_cli::proc(void)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);

    std::string read_str = "";
    std::getline(std::cin, read_str);

    if(read_str.length() > 0)
    {
        std::list<cli_data_c>::iterator iter;
        for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
        {
            if(strncmp(read_str.c_str(), iter->_str.c_str(), iter->_str.length()) == 0)
            {
                log_i("%s : %s\n", __func__, iter->_str.c_str());
                std::list<std::string> param;
                parser(read_str, &param);
                fp func = iter->_func;
                static server_cli *cli_func = this;
                ret_val = (cli_func->*func)(&param);
                break;
            }
        }
    }

    return ret_val;
}

int server_cli::add_cli(fp func, std::string str, std::string help)
{
    int ret_val = RET_OK;
    log_v("%s %s\n", __func__, str.c_str());

    cli_data_c data(func, str, help);
    _cli_menu.push_back(data);

    return ret_val;
}

int server_cli::parser(std::string read_str, std::list<std::string> *param)
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

int server_cli::cli_help(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    printf("==========  cli menu  ========== \n");
    std::list<cli_data_c>::iterator iter;
    for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
    {
        printf("%s : %s\n", iter->_str.c_str(), iter->_help.c_str());
    }
    printf("================================ \n");
    return ret_val;
}

int server_cli::cli_init(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_INIT);
    return ret_val;
}

int server_cli::cli_deinit(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_DEINIT);
    return ret_val;
}

int server_cli::cli_start(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_SERVER_START);
    return ret_val;
}

int server_cli::cli_stop(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_SERVER_STOP);
    return ret_val;
}

int server_cli::cli_helloworld(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    shared_ptr<dat_string> data = make_shared<dat_string>();
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

int server_cli::cli_exit(std::list<std::string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_EXIT);
    return ret_val;
}

