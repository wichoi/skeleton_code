#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../common.h"
#include "../debug/dbg-log.h"
#include "../event/event.h"
#include "../config/config.h"
#include "../thyme-resource.h"
#include "../thyme-payload.h"
#include "../mqtt-handler.h"
#include "cli-menu.h"

typedef struct
{
    char *str;
    u32 idx;
} t_cmd;

static const t_cmd cmd_tbl[] =
{
    { "init",           AE_INIT },
    { "con",            AE_CONNECT },
    { "subs",           AE_SUBSCRIBE },

    { "ae",             TE_AE },
    { "cnt",            TE_CNT },
    { "cin",            TE_CIN },
    { "sub",            TE_SUB },
    { "resp",           TE_RESP },
    { "beat",           TE_BEAT },
    { "dis",            AE_DISCON },

    { "exit",           EV_NONE },
    { "help",           EV_NONE },
};

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

static char one_time_init = 0;
int cli_proc(void)
{
    int ret = PROC_IDLE;
    char buf[128];
    int i = 0;

    if(one_time_init == 0)
    {
        one_time_init = 1;
        cli_help();
    }

    gets(buf);
    for(i = 0; i<sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
    {
        if(strcmp(cmd_tbl[i].str, buf) == 0)
        {
            put_event(cmd_tbl[i].idx, NULL, 0);
            log_V("input cmd : %s\n", cmd_tbl[i].str);
            ret = PROC_CMD;
            break;
        }
        else if(strcmp((char*)"exit", buf) == 0)
        {
            ret = PROC_EXIT;
            break;
        }
        else if(strcmp((char*)"help", buf) == 0)
        {
            cli_help();
            ret = PROC_CMD;
            break;
        }
    }

    return ret;
}

int cli_test_publish_ae(char *data, u32 len)
{
    int rc;
    char payload[PAYLOAD_SZ] = {0,};
    char *topic = get_req_topic();

    log_I("cli_test_publish_ae\n");

    payload_ae(payload, 0, "apro.gw");
    rc = mqtt_publish(topic, payload, MQTT_QOS_0);
    return rc;
}

int cli_test_publish_cnt(char *data, u32 len)
{
    int rc;
    char payload[PAYLOAD_SZ] = {0,};
    char *topic = get_req_topic();

    log_I("cli_test_publish_cnt\n");


    payload_cnt(payload, 0);
    rc = mqtt_publish(topic, payload, MQTT_QOS_0);
    return rc;
}

int cli_test_publish_cin(char *data, u32 len)
{
    int rc;
    char payload[PAYLOAD_SZ] = {0,};
    char *topic = get_req_topic();

    log_I("cli_test_publish_cin\n");

    payload_cin(payload, "0", "0");
    rc = mqtt_publish(topic, payload, MQTT_QOS_0);
    return rc;
}

int cli_test_publish_sub(char *data, u32 len)
{
    int rc;
    char payload[PAYLOAD_SZ] = {0,};
    char *topic = get_req_topic();

    log_I("cli_test_publish_sub\n");

    payload_sub_create(payload, 0);
    rc = mqtt_publish(topic, payload, MQTT_QOS_0);
    return rc;
}

int cli_test_publish_resp(char *data, u32 len)
{
    int rc;
    char *topic = get_noti_resp_topic();
    log_I("cli_test_publish_resp\n");
    rc = mqtt_publish(topic, "test1234", MQTT_QOS_0);
    return rc;
}

int cli_test_publish_heartbeat(char *data, u32 len)
{
    static u32 sequence = 0;
    int rc;
    char payload[128] = {0,};
    char *ae_id = get_ae_id();
    char *topic = get_heartbeat_topic();
    log_I("cli_test_publish_heartbeat\n");
    snprintf(payload, sizeof(payload), "%s:%d", ae_id, sequence++);
    rc = mqtt_publish(topic, payload, MQTT_QOS_0);
    return rc;
}


