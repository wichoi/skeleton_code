#include <iostream>
#include <string.h>

#include <QString>
#include <QTimer>

#include "common.h"
#include "log_service.h"
#include "config_manager.h"
#include "cli_task.h"

cli_task::cli_task(QObject *parent) :
    QObject(parent),
    _thread(),
    _p_main(),
    _cli_menu(),
    _resolution(100),
    _exit_flag(0)
{
}

cli_task::~cli_task()
{
    _exit_flag = 1;
}

void cli_task::task_start()
{
    this->moveToThread(&_thread);
    QObject::connect(&_thread, SIGNAL(started()), this, SLOT(proc()));
    QObject::connect(this, SIGNAL(finished()), &_thread, SLOT(quit()));
    _thread.start();
}

int cli_task::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("cli_task::%s", __func__);

    _p_main = p_main;
    _cli_menu.clear();
    _resolution = resolution;
    add_cli(&cli_task::help, "help", "help");
    add_cli(&cli_task::help, "ls", "help");
    add_cli(&cli_task::init, "init", "init");
    add_cli(&cli_task::deinit, "deinit", "deinit");

    // hello world
    add_cli(&cli_task::help, "=", "=============== test ===============");
    add_cli(&cli_task::helloworld, "hello", "<param_1> <param_2>");
    add_cli(&cli_task::self_test, "selftest", "self test");
    add_cli(&cli_task::config_test, "config_test", "config_test");
    add_cli(&cli_task::timer_test, "timer", "timer_test");
    add_cli(&cli_task::set_timer, "set_timer", "set_timer test");
    add_cli(&cli_task::kill_timer, "kill_timer", "kill_timer test");
    add_cli(&cli_task::print_timer, "print_timer", "print_timer test");

    // exit
    add_cli(&cli_task::restart, "restart", "restart");
    add_cli(&cli_task::exit, "exit", "exit");

    return ret_val;
}

int cli_task::deinit(void)
{
    int ret_val = RET_OK;
    log_d("cli::%s", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _cli_menu.clear();
    return ret_val;
}

int cli_task::proc(void)
{
    int ret_val = RET_OK;
    //log_i("cli::%s", __func__);

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
                    //log_i("%s : %s", __func__, iter->_str.c_str());
                    list<string> param;
                    parser(read_str, &param);
                    fp func = iter->_func;
                    static cli_task *cli_func = this;
                    ret_val = (cli_func->*func)(&param);
                    break;
                }
            }
        }
        QThread::msleep(_resolution);
    }

    emit finished();
    log_i("cli_task::%s exit !!!", __func__);
    return ret_val;
}

int cli_task::add_cli(fp func, string str, string help)
{
    int ret_val = RET_OK;
    //log_d("%s %s", __func__, str.c_str());

    data_c data(func, str,  help);
    _cli_menu.push_back(data);

    return ret_val;
}

int cli_task::parser(string read_str, list<string> *param)
{
    int ret = RET_OK;
    char *ptr = NULL;
    //log_d("%s : %s", __func__, read_str.c_str());

    ptr = strtok((char*)read_str.c_str(), " ");
    while(ptr != NULL)
    {
        char buf[128] = {0,};
        memcpy(buf, ptr, strlen(ptr));
        //log_d("%s", buf);
        param->push_back(buf);
        ptr = strtok(NULL, " ");
    }

    return ret;
}

int cli_task::help(list<string> *param)
{
    int ret_val = RET_OK;
    param;
    log_raw("===============  cli menu  ===============");
    list<data_c>::iterator iter;
    for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
    {
        log_raw("%s : %s", iter->_str.c_str(), iter->_help.c_str());
    }
    log_raw("==========================================");
    return ret_val;
}

int cli_task::init(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    (void)param;
    return ret_val;
}

int cli_task::deinit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    (void)param;
    return ret_val;
}

int cli_task::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    ev;
    return ret_val;
}

int cli_task::on_timer(u32 id)
{
    int ret_val = RET_OK;
    switch(id)
    {
        case timer_service::TID_HELLO_WORLD:
        {
            log_d("%s TID_HELLO_WORLD", __func__);
            //kill_timer(timer_service::TID_HELLO_WORLD);
        }
        break;
        default:
            break;
    }
    return ret_val;
}

int cli_task::helloworld(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    shared_ptr<data_string> data = make_shared<data_string>();
    list<string>::iterator it;
    for(it = param->begin(); it != param->end(); ++it)
    {
        //log_d("%s", it->c_str());
        data->str_val.append(it->c_str());
        data->str_val.append(" ");
    }
    _p_main->event_publish(event_c::CMD_HELLOWORLD, event_c::OP_NONE, data);
    return ret_val;
}

int cli_task::self_test(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    (void)param;
    _p_main->event_publish(event_c::CMD_SELF_TEST);
    return ret_val;
}

int cli_task::config_test(list<string> *param)
{
    int ret_val = RET_OK;
    (void)param;
    QString value = config_manager::instance()->get_model_name();
    log_d("%s get value[%s]", __func__, value.toStdString().c_str());
    QString model_name("ev1001_test");
    config_manager::instance()->set_model_name(model_name);
    log_d("%s set value[%s]", __func__,
          config_manager::instance()->get_model_name().toStdString().c_str());
    return ret_val;
}

int cli_task::timer_test(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    param;
    shared_ptr<QTimer> timer_handle;
    timer_handle = make_shared<QTimer>();
    //QTimer *timer_handle = new QTimer(this);
    connect(timer_handle.get(), SIGNAL(timeout()), this, SLOT(timer_callback()));
    timer_handle->start(1000);
    //QTimer::singleShot(200, this, SLOT(timer_callback()));
    return ret_val;
}

void cli_task::timer_callback()
{
    log_d("%s", __func__);
}

int cli_task::set_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    param;
    _p_main->set_timer(timer_service::TID_HELLO_WORLD, 1000, this);
    return ret_val;
}

int cli_task::kill_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    param;
    _p_main->kill_timer(timer_service::TID_HELLO_WORLD);
    return ret_val;
}

int cli_task::print_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    param;
    _p_main->event_publish(event_c::CMD_TIMER_PRINT);
    return ret_val;
}

int cli_task::restart(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    param;
    _p_main->event_publish(event_c::CMD_RESTART);
    //_p_main->set_timer(timer_service::TID_REBOOT);
    return ret_val;
}

int cli_task::exit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s", __func__);
    param;
    _exit_flag = 1;
    _p_main->event_publish(event_c::CMD_EXIT);
    return ret_val;
}
