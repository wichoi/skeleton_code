#ifndef __CLI_H__
#define __CLI_H__

#include <thread>

#include "common.h"
#include "timer.h"
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
    int init(main_interface *p_cli, u32 resolution = 10);
    int deinit(void);

private:
    int proc(void);
    int add_cli(fp func, string str, string help);
    int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, shared_ptr<dat_c> data = NULL);

private:
    int parser(string read_str, list<string> *param);
    int help(list<string> *param);
    int init(list<string> *param);
    int deinit(list<string> *param);

    // main_interface
    int on_timer(u32 id);

    int helloworld(list<string> *param);
    int exit(list<string> *param);

    // atcmd
    int atcmd_send(list<string> *param);

    // timer
    int set_timer(list<string> *param);
    int kill_timer(list<string> *param);

    // config
    int conf_read(list<string> *param);
    int conf_write(list<string> *param);

private:
    thread _thread;
    main_interface *_p_main;
    list<data_c> _cli_menu;
    u32 _resolution;
    int _exit_flag;
};

#endif
