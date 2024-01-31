#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"
#include "timer_service.h"
#include "evcc_task.h"
#include "evcc_message.h"
#include "tcp_client.h"
#include "udp_client.h"

typedef enum evcc_state_enum
{
    EVCC_ST_NONE            = 0,
    EVCC_ST_INIT            = 1,
    EVCC_ST_IDLE            = 2,
    EVCC_ST_SESSION_START   = 3, // usecase A
    EVCC_ST_SESSION_SETUP   = 4, // usecase B
    EVCC_ST_SERVICE         = 5, // usecase C
    EVCC_ST_AUTHORIZATION   = 6, // usecase D
    EVCC_ST_CHARGING_PARAM  = 7, // usecase E
    EVCC_ST_POWER_DELIVERY  = 8, // usecase F
    EVCC_ST_SESSION_STOP    = 9, // usecase H
    EVCC_ST_DEINIT          = 10,
    EVCC_ST_EXIT            = 11
} evcc_st_e;

typedef struct evcc_cmd_tag
{
    int plug;
    int udp_sdp;
    int tcp_session;
    int service;
} evcc_cmd_t;

static int _exit_flag = 0; // 0(run), 1(exit)
static int _evcc_st = EVCC_ST_NONE;
static evcc_cmd_t _evcc_cmd;

static void evcc_proc(void);
static void evcc_state(void);
static int on_event(const event_data *ev);

int evcc_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 0;
    _evcc_st = EVCC_ST_NONE;
    memset((char*)&_evcc_cmd, 0, sizeof(_evcc_cmd));

    pthread_t evcc_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&evcc_thread, &attr, (void*)evcc_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(evcc_thread);

    event_subscribe(EV_INIT, on_event);
    event_subscribe(EV_DEINIT, on_event);
    event_subscribe(EV_EXIT, on_event);
    event_subscribe(EV_SELFTEST, on_event);

    evcc_msg_init();
    tcp_client_init();
    udp_client_init();

    return ret_val;
}

int evcc_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 1;

    evcc_msg_deinit();
    tcp_client_deinit();
    udp_client_deinit();

    return ret_val;
}

static void evcc_proc(void)
{
    int i = 0;
    char buf[128] = {0,};

    while(_exit_flag == 0)
    {
        evcc_state();
        usleep(1000 * 100);
    }
}

static void evcc_state(void)
{
    int prev_st = _evcc_st;
    switch(_evcc_st)
    {
        case EVCC_ST_NONE:
            if(config_main_state_get() == 2) // MAIN_ST_PROC)
            {
                _evcc_st = EVCC_ST_INIT;
            }
            break;
        case EVCC_ST_INIT:
            memset((char*)&_evcc_cmd, 0, sizeof(_evcc_cmd));
            _evcc_st = EVCC_ST_IDLE;
            break;
        case EVCC_ST_IDLE:
            if(_evcc_cmd.plug == 1)
            {
                _evcc_st = EVCC_ST_SESSION_START;
            }
            break;
        case EVCC_ST_SESSION_START:
            if(_evcc_cmd.udp_sdp == 1)
            {
                _evcc_st = EVCC_ST_SESSION_SETUP;
            }
            break;
        case EVCC_ST_SESSION_SETUP:
            if(_evcc_cmd.tcp_session == 1)
            {
                _evcc_st = EVCC_ST_SERVICE;
            }
            break;
        case EVCC_ST_SERVICE:
            if(_evcc_cmd.service == 1)
            {
                _evcc_st = EVCC_ST_AUTHORIZATION;
            }
            break;
        case EVCC_ST_AUTHORIZATION:
            break;
        case EVCC_ST_CHARGING_PARAM:
            break;
        case EVCC_ST_POWER_DELIVERY:
            break;
        case EVCC_ST_SESSION_STOP:
            break;
        case EVCC_ST_DEINIT:
            break;
        case EVCC_ST_EXIT:
            break;
        default:
            break;
    }

    if(prev_st != _evcc_st)
    {
        if(_evcc_st == EVCC_ST_NONE) log_i("EVCC_ST_NONE\n");
        else if(_evcc_st == EVCC_ST_INIT) log_i("EVCC_ST_INIT\n");
        else if(_evcc_st == EVCC_ST_IDLE) log_i("EVCC_ST_IDLE\n");
        else if(_evcc_st == EVCC_ST_SESSION_START) log_i("EVCC_ST_SESSION_START\n");
        else if(_evcc_st == EVCC_ST_SESSION_SETUP) log_i("EVCC_ST_SESSION_SETUP\n");
        else if(_evcc_st == EVCC_ST_SERVICE) log_i("EVCC_ST_SERVICE\n");
        else if(_evcc_st == EVCC_ST_AUTHORIZATION) log_i("EVCC_ST_AUTHORIZATION\n");
        else if(_evcc_st == EVCC_ST_CHARGING_PARAM) log_i("EVCC_ST_CHARGING_PARAM\n");
        else if(_evcc_st == EVCC_ST_POWER_DELIVERY) log_i("EVCC_ST_POWER_DELIVERY\n");
        else if(_evcc_st == EVCC_ST_SESSION_STOP) log_i("EVCC_ST_SESSION_STOP\n");
        else if(_evcc_st == EVCC_ST_DEINIT) log_i("EVCC_ST_DEINIT\n");
        else if(_evcc_st == EVCC_ST_EXIT) log_i("EVCC_ST_EXIT\n");
        else log_i("EVCC_ST_UNKNOWN\n");
        //config_evcc_state_set(_evcc_st);
    }
}

static void on_timer(const u32 tid)
{
    switch(tid)
    {
        case TID_HELLO:
            log_i("%s TID_HELLO\n", __func__);
            timer_kill(TID_HELLO);
            break;
        default:
            break;
    }
}

static int evcc_selftest(const event_data *ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    timer_set(TID_HELLO, 1000, on_timer);
    return ret_val;
}

static int on_event(const event_data *ev)
{
    int ret_val = RET_OK;
    switch(ev->event)
    {
        case EV_INIT:
            break;
        case EV_DEINIT:
            break;
        case EV_EXIT:
            break;
        case EV_SELFTEST:
            evcc_selftest(ev);
            break;
        default:
            break;
    }
    return ret_val;
}

