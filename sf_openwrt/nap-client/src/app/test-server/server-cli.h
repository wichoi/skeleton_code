#ifndef __SERVER_CLI_H__
#define __SERVER_CLI_H__

#include "main-interface.h"

class server_cli : public main_interface
{
private:
    typedef int (server_cli::*fp)(std::list<std::string>*);
    class cli_data_c
    {
    public:
        fp _func;
        std::string _str;
        std::string _help;

        cli_data_c()
        {
        }

        cli_data_c(fp func, std::string str, std::string help)
        {
            _func = func;
            _str = str;
            _help = help;
        }
    };

public:
    server_cli();
    ~server_cli();
    int init(main_interface *p_cli);
    int deinit(void);
    int proc(void);

private:
    int add_cli(fp func, std::string str, std::string help);

private:
    int parser(std::string read_str, std::list<std::string> *param);
    int cli_help(std::list<std::string> *param);
    int cli_init(std::list<std::string> *param);
    int cli_deinit(std::list<std::string> *param);
    int cli_start(std::list<std::string> *param);
    int cli_stop(std::list<std::string> *param);
    int cli_helloworld(std::list<std::string> *param);
    int cli_exit(std::list<std::string> *param);

private:
    main_interface *_p_main;
    std::list<cli_data_c> _cli_menu;
};

#endif
