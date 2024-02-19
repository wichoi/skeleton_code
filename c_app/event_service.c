#include <stdio.h>
#include <string.h>
#include<pthread.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"

#define Q_INACTIVE          0
#define Q_ACTIVE            1
#define EV_QUEUE_SZ         32
#define EV_SUBSCRIBE_SZ     256

typedef struct _ev_subscribe_item
{
    int event;
    event_fp func;
} ev_subscribe;

typedef struct _ev_subscribe_list
{
    ev_subscribe item[EV_SUBSCRIBE_SZ];
    u8 cnt;
} ev_subs_list;

static ev_subs_list _ev_subs;
static event_data _ev_q[EV_QUEUE_SZ];
static u8 _ev_head = 0;
static u8 _ev_tail = 0;
static u8 _is_active = Q_INACTIVE;
pthread_mutex_t _mtx;

void event_init(void)
{
    pthread_mutex_init(&_mtx, NULL);
    memset((char*)&_ev_subs, 0x0, sizeof(_ev_subs));
    memset(_ev_q, 0x0, sizeof(_ev_q));
    _ev_head = 0;
    _ev_tail = 0;
    _is_active = Q_ACTIVE;
}

void event_deinit(void)
{
    // free all event
    _is_active = Q_INACTIVE;
    pthread_mutex_destroy(&_mtx);
}

int event_subscribe(u32 event, event_fp callback)
{
    int ret = RET_ERR;
    if(_ev_subs.cnt < EV_SUBSCRIBE_SZ)
    {
        _ev_subs.item[_ev_subs.cnt].event = event;
        _ev_subs.item[_ev_subs.cnt].func = callback;
        _ev_subs.cnt++;
        ret = RET_OK;
    }
    return ret;
}

int event_publish(u32 ev, u32 op_code, char* data, u32 len)
{
    int ret = RET_OK;
    if(_is_active == Q_ACTIVE)
    {
        pthread_mutex_lock(&_mtx);
        _ev_head++;
        _ev_head&=0x0F;
        _ev_q[_ev_head].event = ev;
        _ev_q[_ev_head].op_code = op_code;
        _ev_q[_ev_head].len = 0;
        if(len > 0 && len < EV_DATA_SZ)
        {
            _ev_q[_ev_head].len = len;
            memcpy(_ev_q[_ev_head].data, data, len);
        }
        pthread_mutex_unlock(&_mtx);
        log_v("%s [%u]\n", __func__, ev);
    }
    else
    {
        ret = RET_ERR;
        log_e("Event Queue InActive\n");
    }

    return ret;
}

static event_data event_get(void)
{
    pthread_mutex_lock(&_mtx);
    _ev_tail++;
    _ev_tail&=0x0F;
    event_data ev = _ev_q[_ev_tail];
    memset((char*)&_ev_q[_ev_tail], 0, sizeof(event_data));
    pthread_mutex_unlock(&_mtx);
    log_v("%s[%u]\n", __func__, ev.event);
    return ev;
}

int event_proc(void)
{
    int ret = RET_OK;
    if(_ev_head != _ev_tail)
    {
        event_data ev = event_get();
        int i = 0;
        log_v("%s [%u]\n", __func__, ev.event);
        for(i = 0; i < _ev_subs.cnt; i++)
        {
            if(_ev_subs.item[i].event == ev.event)
            {
                ret = _ev_subs.item[i].func(&ev);
            }
        }
    }
    return ret;
}

