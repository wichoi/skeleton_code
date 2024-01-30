#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "log_service.h"
#include "event_service.h"
#include "timer_service.h"
#include "cli_task.h"

typedef void(*cli_fp)(void);

typedef struct
{
    char *str;
    cli_fp func;
} cli_cmd;

static int _exit_flag = 0; // 0(run), 1(exit)

static void cli_proc(void);
static void cli_help(void);
static void cli_hello(void);
static void cli_example(void);
static void cli_selftest(void);
static void cli_set_timer(void);
static void cli_kill_timer(void);
static void cli_tcp_send(void);
static void cli_udp_send(void);
static void cli_exit(void);

static const cli_cmd cmd_tbl[] =
{
    { "ls",             cli_help },
    { "help",           cli_help },
    { "hello",          cli_hello },
    { "example",        cli_example },
    { "selftest",       cli_selftest },
    { "set_timer",      cli_set_timer },
    { "kill_timer",     cli_kill_timer },

    { "========== evcc test ==========", NULL },
    { "tcp_send",       cli_tcp_send },
    { "udp_send",       cli_udp_send },

    { "exit",           cli_exit }
};

int cli_init(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _exit_flag = 0;

    pthread_t cli_thread;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    if(pthread_create(&cli_thread, &attr, (void*)cli_proc, NULL) < 0)
    {
        log_e("pthread_create failed\n");
    }
    pthread_detach(cli_thread);
    return ret_val;
}

int cli_deinit(void)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _exit_flag = 1;
    return ret_val;
}

static void cli_proc(void)
{
    int i = 0;
    char buf[128] = {0,};

    while(_exit_flag == 0)
    {
        memset(buf, 0, sizeof(buf));
        gets(buf);
        for(i = 0; i<sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
        {
            if(strcmp(cmd_tbl[i].str, buf) == 0)
            {
                log_v("input cmd : %s\n", cmd_tbl[i].str);
                cmd_tbl[i].func();
                break;
            }
        }
        usleep(1000 * 100);
    }
}

static void cli_help(void)
{
    printf("\n===== command list =====\n");
    int i;
    for(i = 0; i<sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
    {
        printf("%s\n", cmd_tbl[i].str);
    }
    printf("========================\n");
}

static void cli_hello(void)
{
    log_i("%s\n", __func__);
    event_publish(EV_HELLO, OP_NONE, NULL, 0);
}

static void cli_example(void)
{
    log_i("%s\n", __func__);
    event_publish(EV_EXAMPLE, OP_NONE, NULL, 0);
}

static void cli_selftest(void)
{
    log_i("%s\n", __func__);
    event_publish(EV_SELFTEST, OP_NONE, NULL, 0);
}

static void cli_on_timer(const u32 tid)
{
    switch(tid)
    {
        case TID_HELLO:
            log_i("%s TID_HELLO\n", __func__);
            break;
        default:
            break;
    }
}

static void cli_set_timer(void)
{
    log_i("%s\n", __func__);
    timer_set(TID_HELLO, 1000, cli_on_timer);
}

static void cli_kill_timer(void)
{
    log_i("%s\n", __func__);
    timer_kill(TID_HELLO);
}

static void cli_tcp_send(void)
{
    log_i("%s\n", __func__);
    event_publish(EV_TCP_SEND, OP_NONE, NULL, 0);
}

static void cli_udp_send(void)
{
    log_i("%s\n", __func__);
    event_publish(EV_UDP_SEND, OP_NONE, NULL, 0);
}

static void cli_exit(void)
{
    log_i("%s\n", __func__);
    event_publish(EV_EXIT, OP_NONE, NULL, 0);
}

