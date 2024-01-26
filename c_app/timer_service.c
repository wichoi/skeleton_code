#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"
#include "timer_service.h"

#define TM_LIST_SZ              64

typedef struct _timer_item
{
    u32 timer_id;
    u32 interval;
    u64 start_time;
    u64 latest_time;
    timer_fp callback;
} timer_item_t;

static int _exit_flag = 0; // 0(run), 1(exit)
timer_item_t _tm_list[TM_LIST_SZ] = {0,};
pthread_mutex_t _mtx;

static void timer_proc(void);

int timer_init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _exit_flag = 0;
    memset((char*)&_tm_list, 0, sizeof(timer_item_t) * TM_LIST_SZ);

    pthread_t timer_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_mutex_init(&_mtx, NULL);
    if(pthread_create(&timer_thread, &attr, (void*)timer_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(timer_thread);

    return ret_val;
}

int timer_deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _exit_flag = 1;

    pthread_mutex_lock(&_mtx);
    memset((char*)&_tm_list, 0, sizeof(timer_item_t) * TM_LIST_SZ);
    pthread_mutex_unlock(&_mtx);
    pthread_mutex_destroy(&_mtx);

     return ret_val;
}

int timer_set(u32 id, u32 interval_ms, timer_fp callback)
{
    int ret_val = RET_ERR;
    int i = 0;
    log_v("%s id[%u] interval[%u]\n", __func__, id, interval_ms);

    if(interval_ms < 100)
    {
        log_w("%s failed. minimum interval 100 msec. id[%u] interval[%u]\n", __func__, id, interval_ms);
        return RET_ERR;
    }

    timer_kill(id);

    struct timespec time_spec;
    clock_gettime(CLOCK_MONOTONIC, &time_spec);
    u64 tick_count = time_spec.tv_sec * 1000 + time_spec.tv_nsec / 1000000;

    pthread_mutex_lock(&_mtx);
    for(i = 0; i < TM_LIST_SZ; i++)
    {
        if(_tm_list[i].timer_id == TID_NONE)
        {
            _tm_list[i].timer_id = id;
            _tm_list[i].interval = interval_ms;
            _tm_list[i].start_time = tick_count;
            _tm_list[i].latest_time = tick_count;
            _tm_list[i].callback = callback;
            ret_val = RET_OK;
            break;
        }
    }
    pthread_mutex_unlock(&_mtx);
    return ret_val;
}

int timer_kill(u32 id)
{
    int ret_val = RET_ERR;
    int i = 0;
    log_v("%s\n", __func__);

    pthread_mutex_lock(&_mtx);
    for(i = 0; i < TM_LIST_SZ; i++)
    {
        if(_tm_list[i].timer_id == id)
        {
            _tm_list[i].timer_id = TID_NONE;
            _tm_list[i].interval = 0;
            _tm_list[i].start_time = 0;
            _tm_list[i].latest_time = 0;
            _tm_list[i].callback = NULL;
            ret_val = RET_OK;
            break;
        }
    }
    pthread_mutex_unlock(&_mtx);

    return ret_val;
}

int timer_print(void)
{
    int ret_val = RET_OK;
    int i = 0;
    log_i("%s\n", __func__);
    pthread_mutex_lock(&_mtx);
    for(i = 0; i < TM_LIST_SZ; i++)
    {
        if(_tm_list[i].timer_id != TID_NONE)
        {
            log_i("id[%u] interval[%u]\n", _tm_list[i].timer_id, _tm_list[i].interval);
        }
    }
    pthread_mutex_unlock(&_mtx);
    return ret_val;
}

static void timer_proc(void)
{
    int i = 0;
    bool tid_find = false;
    log_i("%s\n", __func__);
    while(_exit_flag == 0)
    {
        for(i = 0; i < TM_LIST_SZ; i++)
        {
            pthread_mutex_lock(&_mtx);
            if(_tm_list[i].timer_id != TID_NONE)
            {
                struct timespec time_spec;
                clock_gettime(CLOCK_MONOTONIC, &time_spec);
                u64 tick_count = time_spec.tv_sec * 1000 + time_spec.tv_nsec / 1000000;
                s32 interval = tick_count - _tm_list[i].latest_time;
                if(interval >= _tm_list[i].interval)
                {
                    //log_v("%s timer_id[%d]\n", __func__, _tm_list[i].timer_id);
                    _tm_list[i].latest_time += _tm_list[i].interval;
                    //_tm_list[i].callback(_tm_list[i].timer_id);
                    tid_find = true;
                }
                else if(interval < 0)
                {
                    log_w("timer::%s invalid time interval [%d]\n", __func__, interval);
                    _tm_list[i].latest_time = tick_count;
                }
            }
            pthread_mutex_unlock(&_mtx);

            if(tid_find == true)
            {
                _tm_list[i].callback(_tm_list[i].timer_id);
                tid_find = false;
            }
        }
        usleep(1000 * 10);
    }
}

