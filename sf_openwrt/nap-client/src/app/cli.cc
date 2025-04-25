#include <string.h>
#include <sys/file.h>

#include <iostream>
#include <list>

#include "common.h"
#ifdef LINUX_PC_APP
  // do nothing
#else
  #include <fw-manager/fw_manager.h>
  #include <fw-manager/fw_api.h>
#endif

#include "log.h"
#include "event.h"
#include "timer.h"
#include "utils.h"
#include "cli.h"
#include "config-manager.h"

using namespace chrono;

cli::cli() :
    _thread(),
    _p_main(),
    _cli_menu(),
    _resolution(100),
    _exit_flag(0)

{
    log_d("%s\n", __func__);
}

cli::~cli()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int cli::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("cli::%s\n", __func__);

    _p_main = p_main;

    _cli_menu.clear();
    add_cli(&cli::help, "help", "help");
    add_cli(&cli::help, "ls", "help");
    add_cli(&cli::init, "init", "init");
    add_cli(&cli::deinit, "deinit", "deinit");

    // hello world
    add_cli(&cli::help, "=", "=============== test ===============");
    add_cli(&cli::helloworld, "hello", "<param_1> <param_2>");
    add_cli(&cli::sys_call, "system", "system call");

    // timer
    add_cli(&cli::set_timer, "set-timer", "<u32 timer id> <u32 interval ms>");
    add_cli(&cli::kill_timer, "kill-timer", "<u32 timer id> ");
    add_cli(&cli::print_timer, "print-timer", "print all activation timer");

    // config
    add_cli(&cli::conf_read, "conf-read", "nap config read");
    add_cli(&cli::conf_write, "conf-write", "nap config write");
    add_cli(&cli::conf_update, "conf-update", "nap config update");

    // reboot
    add_cli(&cli::conf_reboot, "reboot", "rebooting");
    add_cli(&cli::conf_factory, "factory", "factory reset");
    add_cli(&cli::conf_conf_reset, "conf-reset", "config reset");

    // fwup
    add_cli(&cli::fwup_download, "fwdn", "firmware download test");
    add_cli(&cli::fwup_splite, "fwsp", "firmware splite test");

    // nvram
    add_cli(&cli::nvram_get, "nvget", "nvram get UBOOT_NVRAM");
    add_cli(&cli::nvram_set, "nvset", "nvram set UBOOT_NVRAM");

    // system information
    add_cli(&cli::sys_information, "sysinfo", "system info (mem, cpu, etc...)");

    add_cli(&cli::dump_log_file, "dumplog", "log file dump");

    add_cli(&cli::cleanup, "cleanup", "cleanup");

    // exit
    add_cli(&cli::restart, "restart", "restart");
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

        if(read_str.length() == 0)
        {
            if(access("/data/cli", F_OK) == 0)
            {
                char cmd_buf[128] = {0,};
                FILE *cmd_fd = fopen("/data/cli", "r");
                size_t ret = fread(cmd_buf, sizeof(char), sizeof(cmd_buf) - 1, cmd_fd);
                if(ret > 0)
                {
                    if(cmd_buf[ret - 1] == '\n')
                    {
                        cmd_buf[ret - 1] = NULL;
                    }
                    read_str = cmd_buf;
                }
                fclose(cmd_fd);
                remove("/data/cli");
            }
        }

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

int cli::parser(string read_str, list<string> *param)
{
    int ret = RET_OK;
    char *ptr = NULL;
    char *next_ptr = NULL;
    log_d("%s : %s\n", __func__, read_str.c_str());

    ptr = strtok_r((char*)read_str.c_str(), " ", &next_ptr);
    while(ptr != NULL)
    {
        char buf[128] = {0,};
        memcpy(buf, ptr, strlen(ptr));
        //log_d("%s\n", buf);
        param->push_back(buf);
        ptr = strtok_r(NULL, " ", &next_ptr);
    }

    return ret;
}

int cli::help(list<string> *param)
{
    int ret_val = RET_OK;
    debug_printf("===============  cli menu  ===============\n");
    list<data_c>::iterator iter;
    for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
    {
        debug_printf("%s : %s\n", iter->_str.c_str(), iter->_help.c_str());
    }
    debug_printf("==========================================\n");
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
    switch(id)
    {
    case timer::TID_TEST_GRPC_AGING:
        {
            list<string> param;
            ep_frame(&param);
        }
        break;
    default:
        log_i("%s timer id[%u]\n", __func__, id);
        break;
    }
    return RET_OK;
}

int cli::helloworld(list<string> *param)
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

int cli::sys_call(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    string cmd;
    string result;

    list<string>::iterator it = param->begin();
    ++it;
    while(it != param->end())
    {
        cmd.append(it->c_str());
        if(++it != param->end())
        {
            cmd.append(" ");
        }
    }
    //utils::system_call(cmd, result);
    //log_i("%s\n%s\n%s\n", __func__, cmd.c_str(), result.c_str());
    shared_ptr<dat_string> data = make_shared<dat_string>();
    data->data = cmd;
    _p_main->event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);
    //_p_main->event_publish(event_c::CMD_WATCH_POPEN, event_c::OP_NONE, data);

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
        log_d("%s invalid param\n", __func__);
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
        log_d("%s invalid param\n", __func__);
    }
    return ret_val;
}

int cli::print_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_TIMER_PRINT);
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

int cli::conf_update(list<string> *param)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return ret_val;
}

int cli::conf_reboot(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_REBOOT);
    return ret_val;
}

int cli::conf_factory(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_FACTORY_RESET);
    return ret_val;
}

int cli::conf_conf_reset(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CONFIG_RESET);
    return ret_val;
}

int cli::fwup_download(list<string> *param)
{
    int ret_val = RET_OK;
    return ret_val;
}

int cli::fwup_splite(list<string> *param)
{
    int ret_val = RET_OK;

    return ret_val;
}

int cli::nvram_get(list<string> *param)
{
    int ret_val = RET_OK;
#ifdef LINUX_PC_APP
    // do nothing
#else
    string key = "test_key";
    string value = "";
    utils::nvram_uboot_get(key, value);
    log_i("%s key[%s] value[%s]\n", __func__, key.c_str(), value.c_str());
#endif
    return ret_val;
}

int cli::nvram_set(list<string> *param)
{
    int ret_val = RET_OK;
#ifdef LINUX_PC_APP
    // do nothing
#else
    string key = "test_key";
    string value = "test1234";
    utils::nvram_uboot_set(key, value);
    log_i("%s key[%s] value[%s]\n", __func__, key.c_str(), value.c_str());
#endif
    return ret_val;
}

int cli::sys_information(list<string> *param)
{
    int ret_val = RET_OK;
    string mac("");
    string ip_addr("");
    string mem("");
    string cpu("");
    bool if_st = false;
    bool link_st = false;
    bool vlan_valid = false;
    string conn_ifname = "";
    utils::read_conn_interface(conn_ifname);
    string ip_type = "";
    utils::read_ip_type(ip_type);
    utils::read_mac(mac);
    utils::read_ip_addr(ip_addr, ip_type, conn_ifname);
    utils::read_mem_usage(cpu);
    utils::read_cpu_usage(cpu);
    utils::read_link_state(link_st, conn_ifname);
    utils::read_vlan_table(vlan_valid);
    utils::read_interface_state(if_st, conn_ifname);
    log_i("mac[%s] ip[%s] if_st[%d], link_st[%d], vlan[%d]\n",
        mac.c_str(), ip_addr.c_str(), if_st, link_st, vlan_valid);
    return ret_val;
}

int cli::dump_log_file(list<string> *param)
{
    int ret_val = RET_OK;
    string str_dump  = "";
    utils::dump_log_file(str_dump);
    log_i("%s", str_dump.c_str());
    return ret_val;
}

int cli::cleanup(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);

    //string cmd = "";
    //cmd = "killall -9 nap-watchdog";
    //utils::system_call(cmd);
    //cmd = "killall -9 nap-client";
    //utils::system_call(cmd);
    shared_ptr<dat_string> data = make_shared<dat_string>();
    data->data = "killall -9 nap-watchdog";
    _p_main->event_publish(event_c::CMD_WATCH_SYSTEM, event_c::OP_NONE, data);

    _p_main->event_publish(event_c::CMD_EXIT);

    return ret_val;
}

int cli::restart(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_RESTART);
    return ret_val;
}

int cli::exit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_EXIT);
    return ret_val;
}

