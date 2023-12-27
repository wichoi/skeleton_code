#ifndef TIMER_SERVICE_H
#define TIMER_SERVICE_H

#include <thread>
#include <mutex>
#include <chrono>
#include <list>

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QApplication>

#include "common.h"

using namespace chrono;

class timer_listener
{
public:
    timer_listener() {}
    virtual ~timer_listener() {}

public:
    virtual int on_timer(u32 id) = 0;
};

class timer_service :
    public QObject,
    public timer_listener
{
    Q_OBJECT
public:
    explicit timer_service(QObject *parent = nullptr);
    ~timer_service();
    void task_start(void);

private:
    class timer_data
    {
    public:
        u32 timer_id;
        u32 interval;
        steady_clock::time_point start_time;
        steady_clock::time_point latest_time;
        timer_listener *p_timer;
    };

public slots:
    void timer_proc(void);

signals:
    void finished();

public:
    int init(u32 resolution = 10);
    int deinit(void);
    int set_timer(u32 id, u32 interval_ms, timer_listener *p_timer);
    int kill_timer(u32 id);
    int print_timer(void);
    int on_timer(u32 id);

public:
    enum
    {
        TID_NONE                = 0,

        TID_HELLO_WORLD         = 8000,
        TID_SELF_TEST           = 8001,
        TID_IDLE_TEST           = 8002,
        TID_CHARGING_TEST       = 8003,

        TID_REBOOT              = 9000,
        TID_EXIT                = 9001,
    };

private:
    QThread _thread;
    QMutex _mtx;
    list<timer_data> _tm_q;
    u32 _resolution;
    int _exit_flag;
};

#endif // TIMER_SERVICE_H
