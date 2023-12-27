#include <string.h>
#include <time.h>

#include "log.h"
#include "event.h"
#include "timer.h"

static void *pthread_cb(void *arg)
{
    timer *fp_timer = (timer*)arg;
    fp_timer->proc();
}

timer::timer() :
    _thread(),
    _mtx(),
    _tm_q(),
    _resolution(10),
    _exit_flag(0)
{
    log_d("%s\n", __func__);
}

timer::~timer()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int timer::init(u32 resolution)
{
    int ret_val = RET_OK;
    log_d("%s timer resolution %u ms\n", __func__, resolution);
    _resolution = resolution;

    pthread_mutex_init(&_mtx, NULL);
    if(pthread_create(&_thread, NULL, pthread_cb, (void*)this) < 0)
    {
        log_e("timer thread create failed !!!\n");
    }
    pthread_detach(_thread);

    return ret_val;
}

int timer::deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);

    _exit_flag = 1;

    pthread_mutex_lock(&_mtx);
    _tm_q.clear();
    pthread_mutex_unlock(&_mtx);
    pthread_mutex_destroy(&_mtx);

     return ret_val;
}

int timer::set_timer(u32 id, u32 interval_ms, timer_listener *p_timer)
{
    int ret_val = RET_OK;
    log_v("%s id[%u] interval[%u]\n", __func__, id, interval_ms);

    if(interval_ms < 100)
    {
        log_w("%s failed. minimum interval 100 msec. id[%u] interval[%u]\n", __func__, id, interval_ms);
        return RET_ERROR;
    }

    kill_timer(id);

    struct timespec time_spec;
    clock_gettime(CLOCK_MONOTONIC, &time_spec);
    u64 tick_count = time_spec.tv_sec * 1000 + time_spec.tv_nsec / 1000000;

    pthread_mutex_lock(&_mtx);
    timer_data tm;
    tm.timer_id = id;
    tm.interval = interval_ms;
    tm.start_time = tick_count;
    tm.latest_time = tick_count;
    tm.p_timer = p_timer;
    _tm_q.push_back(tm);
    pthread_mutex_unlock(&_mtx);

    return ret_val;
}

int timer::kill_timer(u32 id)
{
    int ret_val = RET_ERROR;
    log_v("%s\n", __func__);

    pthread_mutex_lock(&_mtx);
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
    pthread_mutex_unlock(&_mtx);

    return ret_val;
}

int timer::print_timer(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);

    pthread_mutex_lock(&_mtx);
    if(_tm_q.size() > 0)
    {
        list<timer_data>::iterator iter;
        for(iter = _tm_q.begin(); iter != _tm_q.end(); ++iter)
        {
            log_i("id[%u] interval[%u]\n", iter->timer_id, iter->interval);
        }
    }
    pthread_mutex_unlock(&_mtx);

    return ret_val;
}

int timer::get_queue_cnt(void)
{
    return (int)(_tm_q.size());
}

int timer::on_timer(u32 id)
{
    log_v("%s\n", __func__);
    return RET_OK;
}

int timer::proc(void)
{
    int ret_val = RET_OK;
    log_v("%s\n", __func__);

    while(_exit_flag == 0)
    {
        pthread_mutex_lock(&_mtx);
        if(_tm_q.size() > 0)
        {
            list<timer_data>::iterator iter;
            for(iter = _tm_q.begin(); iter != _tm_q.end(); ++iter)
            {
                struct timespec time_spec;
                clock_gettime(CLOCK_MONOTONIC, &time_spec);
                u64 tick_count = time_spec.tv_sec * 1000 + time_spec.tv_nsec / 1000000;
                s32 interval = tick_count - iter->latest_time;
                if(interval >= iter->interval)
                {
                    //log_v("timer::%s timer_id[%d]\n", __func__, iter->timer_id);
                    iter->latest_time += iter->interval;
                    iter->p_timer->on_timer(iter->timer_id);
                }
                else if(interval < 0)
                {
                    log_w("timer::%s invalid time interval [%d]\n", __func__, interval);
                    iter->latest_time = tick_count;
                }
            }
        }
        pthread_mutex_unlock(&_mtx);
        usleep(_resolution * 1000);
    }

    return ret_val;
}

