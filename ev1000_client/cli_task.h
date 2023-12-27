#ifndef CLI_TASK_H
#define CLI_TASK_H

#include <QObject>
#include <QThread>

#include "main_interface.h"
#include "event_service.h"

class cli_task :
    public QObject,
    public event_listener,
    public timer_listener
{
    Q_OBJECT
private:
    typedef int (cli_task::*fp)(list<string>*);
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
    explicit cli_task(QObject *parent = nullptr);
    virtual ~cli_task();
    void task_start(void);
    int init(main_interface *p_cli, u32 resolution = 100);
    int deinit(void);

    // main_interface
    int on_event(const event_c &ev);
    int on_timer(u32 id);

public slots:
    int proc(void);

signals:
    void finished();

private:
    int add_cli(fp func, string str, string help);

private:
    int parser(string read_str, list<string> *param);
    int help(list<string> *param);
    int init(list<string> *param);
    int deinit(list<string> *param);
    int helloworld(list<string> *param);
    int self_test(list<string> *param);
    int config_test(list<string> *param);

private:
    int timer_test(list<string> *param);
public slots:
    void timer_callback(void);

private:
    int set_timer(list<string> *param);
    int kill_timer(list<string> *param);
    int print_timer(list<string> *param);

private:
    int restart(list<string> *param);
    int exit(list<string> *param);

private:
    QThread _thread;
    main_interface *_p_main;
    list<data_c> _cli_menu;
    u32 _resolution;
    int _exit_flag;
};

#endif // CLI_TASK_H
