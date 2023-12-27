#ifndef __CLI_H__
#define __CLI_H__

#include <pthread.h>

#include "common.h"
#include "main-interface.h"

class cli :
    public main_interface,
    public timer_listener
{
private:
    typedef int (cli::*fp)(list<string>*);
    class data_c
    {
    public:
        fp _func;
        string _str;
        string _help;

        data_c()
        {
        }

        data_c(fp func, string str, string help)
        {
            _func = func;
            _str = str;
            _help = help;
        }
    };

public:
    cli();
    ~cli();
    int init(main_interface *p_cli, u32 resolution = 100);
    int deinit(void);

public:
    int proc(void);
private:
    int add_cli(fp func, string str, string help);

private:
    int parser(string read_str, list<string> *param);
    int help(list<string> *param);
    int init(list<string> *param);
    int deinit(list<string> *param);

    // main_interface
    int on_timer(u32 id);

    int helloworld(list<string> *param);
    int sys_call(list<string> *param);
    int restart(list<string> *param);
    int exit(list<string> *param);

    // timer
    int set_timer(list<string> *param);
    int kill_timer(list<string> *param);
    int print_timer(list<string> *param);

    // config
    int conf_read(list<string> *param);
    int conf_write(list<string> *param);
    int conf_update(list<string> *param);

    // reboot
    int cmd_reboot(list<string> *param);
    int cmd_factory(list<string> *param);
    int cmd_reset(list<string> *param);

    // test command
    int test_event(list<string> *param);
    int test_nvram_get(list<string> *param);
    int test_nvram_set(list<string> *param);
    int test_base64(list<string> *param);
    int test_crypto(list<string> *param);
    int test_hex_to_char(list<string> *param);
    int test_set_modem_config(list<string> *param);
    int test_set_delete_token(list<string> *param);
    int test_set_delete_key(list<string> *param);
    int test_set_server_url(list<string> *param);
    int test_set_loglevel(list<string> *param);
    int test_ip_change(list<string> *param);
    int test_random_time(list<string> *param);
    int test_hostbyname(list<string> *param);
    int test_modem_sms(list<string> *param);
    int test_modem_cclk(list<string> *param);

    // fwup
    int fwup_download(list<string> *param);
    int fwup_splite(list<string> *param);
    int fwup_router(list<string> *param);
    int fwup_modem(list<string> *param);
    int fwup_atcmd(list<string> *param);

    // cloud
    int cloud_auth(list<string> *param);
    int cloud_sauth(list<string> *param);
    int cloud_boot(list<string> *param);
    int cloud_event_inform(list<string> *param);
    int cloud_report(list<string> *param);
    int cloud_heartbeat(list<string> *param);
    int cloud_getkey(list<string> *param);
    int cloud_keyupdated(list<string> *param);
    int cloud_getparam(list<string> *param);
    int cloud_paramupdated(list<string> *param);
    int cloud_getfwup(list<string> *param);
    int cloud_fwup(list<string> *param);

    // at command
    int at_command(list<string> *param);

    // system information
    int sys_information(list<string> *param);

    int dump_log_file(list<string> *param);

private:
    pthread_t _thread;
    main_interface *_p_main;
    list<data_c> _cli_menu;
    u32 _resolution;
    int _exit_flag;
};

#endif
