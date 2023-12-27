#ifndef __SERVER_PROC_H__
#define __SERVER_PROC_H__

#include "event.h"
#include "timer.h"
#include "main-interface.h"
#include "server-cli.h"
#include "server-handler.h"

class server_proc : public main_interface
{
private:
    typedef int (server_proc::*fp)(const event_c &ev);
    class func_list_c
    {
    public:
        int _cmd;
        fp _func;
        std::string _str;
        func_list_c() {}
        func_list_c(int cmd, fp func, std::string str)
        {
            _cmd = cmd;
            _func = func;
            _str = str;
        }
    };

public:
    server_proc();
    ~server_proc();
    int init(void);
    int deinit(const event_c &ev);
    int proc(void);

private:
    int start_server(const event_c &ev);
    int stop_server(const event_c &ev);
    int hello_world(const event_c &ev);
    int exit(const event_c &ev);
    int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, shared_ptr<dat_c> data = NULL);
    int event_proc(void);
    int add_func(int cmd, fp func, std::string str);

private:
    event_queue _event_q;
    timer _timer;
    server_cli _cli;
    server_handler _handle;
    std::list<func_list_c> _func_list;
};

#endif
