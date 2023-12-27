#include "common.h"
#include "log_service.h"
#include "timer_service.h"

timer_service::timer_service(QObject *parent) :
    QObject(parent),
    _thread(),
    _mtx(),
    _tm_q(),
    _resolution(10),
    _exit_flag(0)
{
    //log_d("%s", __func__);
}

timer_service::~timer_service()
{
    //log_d("%s", __func__);
    _exit_flag = 1;
}

void timer_service::task_start()
{
    this->moveToThread(&_thread);
    QObject::connect(&_thread, SIGNAL(started()), this, SLOT(timer_proc()));
    QObject::connect(this, SIGNAL(finished()), &_thread, SLOT(quit()));
    _thread.start();
}

int timer_service::init(u32 resolution)
{
    int ret_val = RET_OK;
    log_i("%s timer resolution %u ms", __func__, resolution);
    _resolution = resolution;
    return ret_val;
}

int timer_service::deinit(void)
{
    int ret_val = RET_OK;
    log_i("timer_service::%s", __func__);

    _exit_flag = 1;

    _mtx.lock();
    _tm_q.clear();
    _mtx.unlock();

    return ret_val;
}

int timer_service::set_timer(u32 id, u32 interval_ms, timer_listener *p_timer)
{
    int ret_val = RET_OK;
    //log_d("%s id[%u] interval[%u]", __func__, id, interval_ms);

    if(interval_ms < 100)
    {
        log_w("%s failed. minimum interval 100 msec. id[%u] interval[%u]", __func__, id, interval_ms);
        return RET_ERROR;
    }

    kill_timer(id);

    _mtx.lock();
    timer_data tm;
    tm.timer_id = id;
    tm.interval = interval_ms;
    tm.start_time = steady_clock::now();
    tm.latest_time = steady_clock::now();
    tm.p_timer = p_timer;
    _tm_q.push_back(tm);
    _mtx.unlock();

    return ret_val;
}

int timer_service::kill_timer(u32 id)
{
    int ret_val = RET_ERROR;
    //log_d("%s", __func__);

    _mtx.lock();
    if(_tm_q.size() > 0)
    {
        list<timer_data>::iterator iter;
        for(iter = _tm_q.begin(); iter != _tm_q.end(); )
        {
            if(iter->timer_id == id)
            {
                iter = _tm_q.erase(iter);
                ret_val = RET_OK;
                break;
            }
            else
            {
                ++iter;
            }
        }
    }
    _mtx.unlock();

    return ret_val;
}

int timer_service::print_timer(void)
{
    int ret_val = RET_OK;
    log_i("%s", __func__);

    _mtx.lock();
    if(_tm_q.size() > 0)
    {
        list<timer_data>::iterator iter;
        for(iter = _tm_q.begin(); iter != _tm_q.end(); ++iter)
        {
            log_i("id[%u] interval[%u]", iter->timer_id, iter->interval);
        }
    }
    _mtx.unlock();

    return ret_val;
}

int timer_service::on_timer(u32 id)
{
    //log_d("%s", __func__);
    (void)id;
    return RET_OK;
}

void timer_service::timer_proc(void)
{
    while(_exit_flag == 0)
    {
        _mtx.lock();
        if(_tm_q.size() > 0)
        {
            list<timer_data>::iterator iter;
            for(iter = _tm_q.begin(); iter != _tm_q.end(); ++iter)
            {
                steady_clock::time_point now_time = steady_clock::now();
                milliseconds interval = duration_cast<milliseconds>(now_time - iter->latest_time);
                if(interval.count() >= iter->interval)
                {
                    //log_v("timer::%s timer_id[%d]", __func__, iter->timer_id);
                    iter->latest_time += milliseconds(iter->interval);
                    iter->p_timer->on_timer(iter->timer_id);
                }
                else if(interval.count() < 0)
                {
                    log_w("timer::%s invalid time interval [%d]", __func__, interval.count());
                    iter->latest_time = now_time;
                }
            }
        }
        _mtx.unlock();
        this_thread::sleep_for(milliseconds(_resolution));
    }

    emit finished();
    log_i("%s exit !!!", __func__);
}
