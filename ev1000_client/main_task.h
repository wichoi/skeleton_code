#ifndef MAIN_TASK_H
#define MAIN_TASK_H

#include <QObject>
#include <QThread>
#include <QApplication>

#include "main_interface.h"
#include "event_service.h"
#include "config_service.h"
#include "cli_task.h"
#include "media_task.h"
#include "main_window.h"

class main_task :
    public QObject,
    public main_interface,
    public event_listener,
    public timer_listener
{
    Q_OBJECT
public:
    explicit main_task(QObject *parent = nullptr);
    virtual ~main_task();
    void task_start(const QApplication &a);

private:
    int main_init(void);
    int main_deinit(void);

signals:
    void finished(void);

public slots:
    void main_proc(void);

private:
    // event interface
    int event_subscribe(u32 cmd, event_listener *p_listener);
    int event_publish(u32 cmd, u32 op_code = event_c::OP_NONE, shared_ptr<ev_data> data = NULL);
    int on_event(const event_c &ev);

    // timer interface
    int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer);
    int kill_timer(u32 id);
    int on_timer(u32 id);
    int print_timer(const event_c &ev);

private:
    int main_event(void);
    int hello_world(const event_c &ev);
    int exit(const event_c &ev);

public:
    int main_state(void);

public:
    enum
    {
        MAIN_ST_NONE = 0,
        MAIN_ST_START = 1,
        MAIN_ST_INIT = 2,
        MAIN_ST_IDLE = 3,
        MAIN_ST_DEINIT = 4,
        MAIN_ST_EXIT = 5,
        MAIN_ST_MAX = 6,
    };

private:
    QThread _main_thread;
    int _state;
    config_service _config_handle;
    event_service _event_handle;
    timer_service _timer_handle;
    cli_task _cli_handle;
    media_task _media_handle;
    main_window _ui_handle;
};

#endif // MAIN_TASK_H
