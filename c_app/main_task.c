#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "log_service.h"
#include "config_manager.h"
#include "timer_service.h"
#include "event_service.h"
#include "main_task.h"
#include "cli_task.h"
#include "secc_task.h"
#include "evcc_task.h"
#include "utils.h"

#include "main.h"

typedef enum main_state_enum
{
    MAIN_ST_NONE    = 0,
    MAIN_ST_INIT    = 1,
    MAIN_ST_PROC    = 2,
    MAIN_ST_DEINIT  = 3,
    MAIN_ST_EXIT    = 4
} main_st_e;

static int _exit_flag = 0; // 0(run), 1(exit)
static int _main_st = MAIN_ST_NONE;
static int _is_evcc = 0;

static int on_event(const event_data *ev);

int main_init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _exit_flag = 0;
    _main_st = MAIN_ST_NONE;

    event_subscribe(EV_INIT, on_event);
    event_subscribe(EV_DEINIT, on_event);
    event_subscribe(EV_EXIT, on_event);
    event_subscribe(EV_HELLO, on_event);
    event_subscribe(EV_EXAMPLE, on_event);
    event_subscribe(EV_SELFTEST, on_event);
    return ret_val;
}

int main_deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);

    config_deinit();
    event_deinit();
    timer_deinit();
    cli_deinit();
    utils_deinit();

    if(_is_evcc == 1)
    {
        evcc_deinit();
    }
    else
    {
        secc_deinit();
    }

    _exit_flag = 1;
    return ret_val;
}

int main_start(int is_evcc)
{
    int ret_val = RET_OK;
    debug_init(LOG_ENABLE, PRINT_INFO, NULL);
    log_i("%s\n", __func__);
    _is_evcc = is_evcc;

    config_init();
    event_init();
    timer_init();
    main_init();
    cli_init();
    utils_init();

    if(_is_evcc == 1)
    {
        evcc_init();
    }
    else
    {
        secc_init();
    }

    return ret_val;
}

int main_proc(void)
{
    int prev_st = MAIN_ST_NONE;
    log_i("%s\n", __func__);
    while(_exit_flag == 0)
    {
        prev_st = _main_st;
        switch(_main_st)
        {
            case MAIN_ST_NONE:
                _main_st = MAIN_ST_INIT;
                break;
            case MAIN_ST_INIT:
                _main_st = MAIN_ST_PROC;
                break;
            case MAIN_ST_PROC:
                if(event_proc() == RET_EXIT)
                {
                    _main_st = MAIN_ST_DEINIT;
                }
                break;
            case MAIN_ST_DEINIT:
                _main_st = MAIN_ST_EXIT;
                break;
            case MAIN_ST_EXIT:
                _exit_flag = 1;
                break;
        }

        if(prev_st != _main_st)
        {
            if(_main_st == MAIN_ST_NONE) log_i("MAIN_ST_NONE\n");
            else if(_main_st == MAIN_ST_INIT) log_i("MAIN_ST_INIT\n");
            else if(_main_st == MAIN_ST_PROC) log_i("MAIN_ST_PROC\n");
            else if(_main_st == MAIN_ST_DEINIT) log_i("MAIN_ST_DEINIT\n");
            else if(_main_st == MAIN_ST_EXIT) log_i("MAIN_ST_EXIT\n");
            else log_i("MAIN_ST_UNKNOWN\n");
            config_main_state_set(_main_st);
        }
        usleep(1000 * 10);
    }
    log_i("%s exit !!!\n", __func__);
    return _exit_flag;
}

static void on_timer(const u32 tid)
{
    switch(tid)
    {
        case TID_HELLO:
            log_i("%s TID_HELLO\n", __func__);
            break;
        case TID_TIMER_TEST1:
            log_i("%s TID_TIMER_TEST1\n", __func__);
            timer_kill(TID_TIMER_TEST1);
            break;
        case TID_TIMER_TEST2:
            log_i("%s TID_TIMER_TEST2\n", __func__);
            timer_kill(TID_TIMER_TEST2);
            break;
        case TID_TIMER_TEST3:
            log_i("%s TID_TIMER_TEST3\n", __func__);
            timer_kill(TID_TIMER_TEST3);
            break;
        default:
            break;
    }
}

static int main_exit(const event_data *ev)
{
    int ret_val = RET_EXIT;
    log_i("%s\n", __func__);
    main_deinit();
    return ret_val;
}

static int main_hello(const event_data *ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

static int main_exam(const event_data *ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    main_example(0, NULL);
    return ret_val;
}

static int main_selftest(const event_data *ev)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    timer_set(TID_TIMER_TEST1, 1000, on_timer);
    timer_set(TID_TIMER_TEST2, 3000, on_timer);
    timer_set(TID_TIMER_TEST3, 5000, on_timer);
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
            ret_val = main_exit(ev);
            break;
        case EV_HELLO:
            main_hello(ev);
            break;
        case EV_EXAMPLE:
            main_exam(ev);
            break;
        case EV_SELFTEST:
            main_selftest(ev);
            break;
        default:
            break;
    }
    return ret_val;
}

