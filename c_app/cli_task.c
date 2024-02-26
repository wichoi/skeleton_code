#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "common.h"
#include "log_service.h"
#include "tools_list.h"
#include "utils.h"
#include "event_service.h"
#include "timer_service.h"
#include "cli_task.h"

typedef void(*cli_fp)(const char*);

typedef struct
{
    char *str;
    cli_fp func;
} cli_cmd;

static int _exit_flag = 0; // 0(run), 1(exit)

static void cli_proc(void);
static void cli_help(const char *input);
static void cli_hello(const char *input);
static void cli_example(const char *input);
static void cli_selftest(const char *input);
static void cli_set_timer(const char *input);
static void cli_kill_timer(const char *input);
static void cli_tools_list(const char *input);

static void cli_secc_plug(const char *input);
static void cli_secc_udp_sdp(const char *input);
static void cli_secc_supported_app(const char *input);
static void cli_secc_service(const char *input);
static void cli_secc_cert_update(const char *input);
static void cli_secc_cert_install(const char *input);
static void cli_secc_error(const char *input);
static void cli_evcc_udp_sdp(const char *input);
static void cli_evcc_supported_app(const char *input);
static void cli_evcc_session_setup(const char *input);
static void cli_evcc_service_discover(const char *input);
static void cli_evcc_service_detail(const char *input);
static void cli_evcc_payment_select(const char *input);
static void cli_evcc_payment_detail(const char *input);
static void cli_evcc_authorization(const char *input);
static void cli_evcc_charging_param(const char *input);
static void cli_evcc_power_delivery(const char *input);
static void cli_evcc_cert_update(const char *input);
static void cli_evcc_certi_install(const char *input);
static void cli_evcc_session_stop(const char *input);
static void cli_evcc_charging_status(const char *input);
static void cli_evcc_metering_receipt(const char *input);
static void cli_evcc_cable_check(const char *input);
static void cli_evcc_pre_charge(const char *input);
static void cli_evcc_current_demand(const char *input);
static void cli_evcc_welding_detection(const char *input);

static void cli_tcp_send(const char *input);
static void cli_udp_send(const char *input);

static void cli_exit(const char *input);

static const cli_cmd cmd_tbl[] =
{
    { "ls",                 cli_help },
    { "help",               cli_help },
    { "hello",              cli_hello },
    { "example",            cli_example },
    { "selftest",           cli_selftest },
    { "set_timer",          cli_set_timer },
    { "kill_timer",         cli_kill_timer },
    { "list_test",          cli_tools_list },

    { "========== secc cmd ==========", NULL },
    { "secc_plug",          cli_secc_plug },
    { "secc_sdp",           cli_secc_udp_sdp },
    { "secc_support",       cli_secc_supported_app },
    { "secc_service",       cli_secc_service },
    { "secc_cert_update",   cli_secc_cert_update },
    { "secc_cert_install",  cli_secc_cert_update },

    { "secc_error",         cli_secc_error },

    { "========== evcc cmd ==========", NULL },
    { "evcc_sdp",           cli_evcc_udp_sdp },
    { "evcc_support",       cli_evcc_supported_app },
    { "evcc_session",       cli_evcc_session_setup },
    { "evcc_serv_discover", cli_evcc_service_discover },
    { "evcc_serv_detail",   cli_evcc_service_detail },
    { "evcc_pay_sel",       cli_evcc_payment_select },
    { "evcc_pay_detail",    cli_evcc_payment_detail },
    { "evcc_auth",          cli_evcc_authorization },
    { "evcc_charge_param",  cli_evcc_charging_param },
    { "evcc_power",         cli_evcc_power_delivery },
    { "evcc_cert_update",   cli_evcc_cert_update },
    { "evcc_cert_install",  cli_evcc_certi_install },
    { "evcc_stop",          cli_evcc_session_stop },
    { "evcc_charging",      cli_evcc_charging_status },
    { "evcc_metering",      cli_evcc_metering_receipt },
    { "evcc_cable",         cli_evcc_cable_check },
    { "evcc_pre",           cli_evcc_pre_charge },
    { "evcc_demand",        cli_evcc_current_demand },
    { "evcc_weld",          cli_evcc_welding_detection },

    { "========== socket test ==========", NULL },
    { "tcp_send",           cli_tcp_send },
    { "udp_send",           cli_udp_send },

    { "exit",               cli_exit }
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
            if(strncmp(cmd_tbl[i].str, buf, strlen(cmd_tbl[i].str)) == 0)
            {
                log_v("input cmd : %s\n", cmd_tbl[i].str);
                cmd_tbl[i].func(buf);
                break;
            }
        }
        usleep(1000 * 100);
    }
}

static void cli_help(const char *input)
{
    debug_printf("\n===== command list =====\n");
    int i;
    for(i = 0; i<sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
    {
        debug_printf("%s\n", cmd_tbl[i].str);
    }
    debug_printf("========================\n");
}

static void cli_hello(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_HELLO, OP_NONE, NULL, 0);
}

static void cli_example(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EXAMPLE, OP_NONE, NULL, 0);
}

static void cli_selftest(const char *input)
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

static void cli_set_timer(const char *input)
{
    log_i("%s\n", __func__);
    timer_set(TID_HELLO, 1000, cli_on_timer);
}

static void cli_kill_timer(const char *input)
{
    log_i("%s\n", __func__);
    timer_kill(TID_HELLO);
}

static void cli_tools_list(const char *input)
{
    log_i("%s\n", __func__);

    int i = 0;
    list_item_t *item;
    list_t *list_handle = (list_t*)list_new();
    list_handle->push_back(list_handle, "0.TEST1234", strlen("1.TEST1234"));
    list_handle->push_back(list_handle, "1.TEMP", strlen("1.TEMP"));
    list_handle->push_back(list_handle, "2.DATA12345", strlen("2.DATA12345"));
    list_handle->push_back(list_handle, "3.dummy", strlen("3.dummy"));
    list_handle->push_back(list_handle, "4.1234567890", strlen("4.1234567890"));
    list_handle->push_back(list_handle, "5.1234567890", strlen("5.1234567890"));

    log_i("list_handle->cnt[%d]\n", list_handle->cnt);
    for(i = 0; i <list_handle->cnt; i++)
    {
        item = list_handle->get(list_handle, i);
        log_i("index[%d] len[%02d] data[%s]\n", i, item->len, item->data);
    }

    list_handle->erase(list_handle, 5);
    list_handle->erase(list_handle, 0);
    list_handle->erase(list_handle, 2);

    log_i("list_handle->cnt[%d]\n", list_handle->cnt);
    for(i = 0; i <list_handle->cnt; i++)
    {
        item = list_handle->get(list_handle, i);
        log_i("index[%d] item->data[%s]\n", i, item->data);
    }

    list_handle->clear(list_handle);
    log_i("%s, list_handle->cnt[%d]\n", __func__, list_handle->cnt);

    list_handle->push_back(list_handle, "6.1234567890", strlen("6.1234567890"));
    log_i("list_handle->cnt[%d]\n", list_handle->cnt);
    for(i = 0; i <list_handle->cnt; i++)
    {
        item = list_handle->get(list_handle, i);
        log_i("index[%d] len[%02d] data[%s]\n", i, item->len, item->data);
    }

    list_handle->free(list_handle);
    list_handle = NULL;
    log_i("list_handle[%X]\n", list_handle);
}

static void cli_secc_plug(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_PLUG, OP_NONE, NULL, 0);
}

static void cli_secc_udp_sdp(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_SESSION_START, OP_NONE, NULL, 0);
}

static void cli_secc_supported_app(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_SUPPORTED_APP, OP_NONE, NULL, 0);
}

static void cli_secc_service(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_SERVICE_DISCOVER, OP_NONE, NULL, 0);
}

static void cli_secc_cert_update(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_CERT_UPDATE, OP_NONE, NULL, 0);
}

static void cli_secc_cert_install(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_CERT_INSTALL, OP_NONE, NULL, 0);
}

static void cli_secc_error(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_SECC_ERROR, OP_NONE, NULL, 0);
}

static void cli_evcc_udp_sdp(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_SESSION_START, OP_NONE, NULL, 0);
}

static void cli_evcc_supported_app(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_SUPPORTED_APP, OP_NONE, NULL, 0);
}

static void cli_evcc_session_setup(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_SESSION_SETUP, OP_NONE, NULL, 0);
}

static void cli_evcc_service_discover(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_SERVICE_DISCOVER, OP_NONE, NULL, 0);
}

static void cli_evcc_service_detail(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_SERVICE_DETAIL, OP_NONE, NULL, 0);
}

static void cli_evcc_payment_select(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_PAYMENT_SEL, OP_NONE, NULL, 0);
}

static void cli_evcc_payment_detail(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_PAYMENT_DETAIL, OP_NONE, NULL, 0);
}

static void cli_evcc_authorization(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_AUTHORIZATION, OP_NONE, NULL, 0);
}

static void cli_evcc_charging_param(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_CHARGING_PARAM, OP_NONE, NULL, 0);
}

static void cli_evcc_power_delivery(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_POWER_DELIVERY, OP_NONE, NULL, 0);
}

static void cli_evcc_cert_update(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_CERT_UPDATE, OP_NONE, NULL, 0);
}

static void cli_evcc_certi_install(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_CERT_INSTALL, OP_NONE, NULL, 0);
}

static void cli_evcc_session_stop(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_SESSION_STOP, OP_NONE, NULL, 0);
}

static void cli_evcc_charging_status(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_CHARGING_STATUS, OP_NONE, NULL, 0);
}

static void cli_evcc_metering_receipt(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_METERING_RECEIPT, OP_NONE, NULL, 0);
}

static void cli_evcc_cable_check(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_CABLE_CHECK, OP_NONE, NULL, 0);
}

static void cli_evcc_pre_charge(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_PRE_CHARGE, OP_NONE, NULL, 0);
}

static void cli_evcc_current_demand(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_CURRENT_DEMAND, OP_NONE, NULL, 0);
}

static void cli_evcc_welding_detection(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EVCC_WELDING_DETECTION, OP_NONE, NULL, 0);
}

static void cli_tcp_send(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_TCP_SEND, OP_NONE, NULL, 0);
}

static void cli_udp_send(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_UDP_SEND, OP_NONE, NULL, 0);
}

static void cli_exit(const char *input)
{
    log_i("%s\n", __func__);
    event_publish(EV_EXIT, OP_NONE, NULL, 0);
}

