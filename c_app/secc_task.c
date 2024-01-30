#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"
#include "timer_service.h"
#include "secc_task.h"
#include "tcp_server.h"
#include "udp_server.h"

typedef enum secc_state_enum
{
    SECC_ST_NONE            = 0,
    SECC_ST_INIT            = 1,
    SECC_ST_IDLE            = 2,
    SECC_ST_SESSION_START   = 3, // usecase A
    SECC_ST_SESSION_SETUP   = 4, // usecase B
    SECC_ST_SERVICE         = 5, // usecase C
    SECC_ST_AUTHORIZATION   = 6, // usecase D
    SECC_ST_CHARGING_PARAM  = 7, // usecase E
    SECC_ST_POWER_DELIVERY  = 8, // usecase F
    SECC_ST_SESSION_STOP    = 9, // usecase H
    SECC_ST_DEINIT          = 10,
    SECC_ST_EXIT            = 11
} secc_st_e;

static int _exit_flag = 0; // 0(run), 1(exit)
static int _secc_st = SECC_ST_NONE;

static void secc_proc(void);
static void secc_state(void);
static int on_event(const event_data *ev);

int secc_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 0;
    _secc_st = SECC_ST_NONE;

    pthread_t secc_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&secc_thread, &attr, (void*)secc_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(secc_thread);

    event_subscribe(EV_INIT, on_event);
    event_subscribe(EV_DEINIT, on_event);
    event_subscribe(EV_EXIT, on_event);
    event_subscribe(EV_SELFTEST, on_event);

    tcp_server_init();
    udp_server_init();

    return ret_val;
}

int secc_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    _exit_flag = 1;

    tcp_server_deinit();
    udp_server_deinit();

    return ret_val;
}

static void secc_proc(void)
{
    int i = 0;
    char buf[128] = {0,};

    while(_exit_flag == 0)
    {
        secc_state();
        usleep(1000 * 100);
    }
}

static void secc_state(void)
{
    int prev_st = _secc_st;
    switch(_secc_st)
    {
        case SECC_ST_NONE:
            if(config_main_state_get() == 2) // MAIN_ST_PROC)
            {
                _secc_st = SECC_ST_INIT;
            }
            break;
        case SECC_ST_INIT:
            _secc_st = SECC_ST_IDLE;
            break;
        case SECC_ST_IDLE:
            // todo plug connect
            break;
        case SECC_ST_SESSION_START:
            // todo udp socket listen
            break;
        case SECC_ST_SESSION_SETUP:
            // todo tcp socket listen
            break;
        case SECC_ST_SERVICE:
            break;
        case SECC_ST_AUTHORIZATION:
            break;
        case SECC_ST_CHARGING_PARAM:
            break;
        case SECC_ST_POWER_DELIVERY:
            break;
        case SECC_ST_SESSION_STOP:
            break;
        case SECC_ST_DEINIT:
            break;
        case SECC_ST_EXIT:
            break;
        default:
            break;
    }

    if(prev_st != _secc_st)
    {
        if(_secc_st == SECC_ST_NONE) log_i("SECC_ST_NONE\n");
        else if(_secc_st == SECC_ST_INIT) log_i("SECC_ST_INIT\n");
        else if(_secc_st == SECC_ST_IDLE) log_i("SECC_ST_IDLE\n");
        else if(_secc_st == SECC_ST_SESSION_START) log_i("SECC_ST_SESSION_START\n");
        else if(_secc_st == SECC_ST_SESSION_SETUP) log_i("SECC_ST_SESSION_SETUP\n");
        else if(_secc_st == SECC_ST_SERVICE) log_i("SECC_ST_SERVICE\n");
        else if(_secc_st == SECC_ST_AUTHORIZATION) log_i("SECC_ST_AUTHORIZATION\n");
        else if(_secc_st == SECC_ST_CHARGING_PARAM) log_i("SECC_ST_CHARGING_PARAM\n");
        else if(_secc_st == SECC_ST_POWER_DELIVERY) log_i("SECC_ST_POWER_DELIVERY\n");
        else if(_secc_st == SECC_ST_SESSION_STOP) log_i("SECC_ST_SESSION_STOP\n");
        else if(_secc_st == SECC_ST_DEINIT) log_i("SECC_ST_DEINIT\n");
        else if(_secc_st == SECC_ST_EXIT) log_i("SECC_ST_EXIT\n");
        else log_i("SECC_ST_UNKNOWN\n");
        config_secc_state_set(_secc_st);
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

static int secc_selftest(const event_data *ev)
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
            secc_selftest(ev);
            break;
        default:
            break;
    }
    return ret_val;
}

