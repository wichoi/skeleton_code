#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "common.h"
#include "debug/dbg-log.h"
#include "timer/timer.h"
#include "event/event.h"
#include "config/config.h"
#include "state/state.h"
#include "cli/cli-menu.h"
#include "thyme-payload.h"
#include "thyme-resource.h"
#include "mqtt-handler.h"
#include "tas-ble/ble-tas.h"
#include "tas-zigbee/zigbee-tas.h"
#include "tas-zwave/zwave-tas.h"

static int thyme_subscribe(char *data, u32 len);

static void onConnected(int result)
{
    log_I("onConnected() : %d\n", result); // 0:disconnect, 1:connect
    mqtt_state(MQTT_SUBSCRIBE);
}

static void onReconnect(char *cause)
{
    log_I("onReconnect() : %s\n", cause);

    if(strstr((char*)cause, (char*)"reconnect"))
    {
        mqtt_state(MQTT_RECONNECTED);
    }
}

static void onSubscribed(int result)
{
    log_I("onSubscribed() : %d\n", result);
    mqtt_state(MQTT_CONNECTED);

}

static void onDisconnected(int result)
{
    log_I("onDisconnected() : %d\n", result);
    mqtt_state(MQTT_DISCONNECT);
}

static void onConnectionLost(char* cause)
{
    log_I("onConnectionLost() : %s\n", cause);
    mqtt_state(MQTT_DISCONNECT);
}

static void onMessageDelivered(int token)
{
    log_I("onMessageDelivered() : %d\n", token);
}

static void onMessageArrived(char* topic, char* msg, int msgLen)
{
    log_I("onMessageArrived() topic : %s\n", topic);
    log_I("onMessageArrived() msg : %s\n", msg);

    if(msg == NULL || msgLen < 1)
    {
        return;
    }
#if 0
    char payload[2048] = "";
    memcpy(payload, msg, msgLen);
    log_I("payload : %s\n", payload);
    cJSON* root = cJSON_Parse(payload);
    if(!root) return;
    cJSON_Delete(root);
#endif
}

static void onBleTasCb(char *data, u32 len)
{
    log_I("onBleTasCb\n");
    put_event(BL_GET, data, len);
}

static void onZigbeeTasCb(char *data, u32 len)
{
    log_I("onBleTasCb\n");
    put_event(ZI_GET, data, len);
}

static void onZwaveTasCb(char *data, u32 len)
{
    log_I("onBleTasCb\n");
    put_event(ZW_GET, data, len);
}

void test(int test)
{
    log_I("test [%d]\n", test);
//    return 0;
}

static int thyme_init(char *data, u32 len)
{
    int rc = 0;
    log_I("thyme_init\n");
    init_config();
    res_init();
    ble_tas_init(onBleTasCb);
    zigbee_tas_init(onZigbeeTasCb);
    zwave_tas_init(onZwaveTasCb);
//    mqtt_state(MQTT_INIT);

//    set_timer(0, 250000, test);
    set_timer(1, 0, test);


    return rc;
}

void thyme_destroy(void)
{
    // todo free, destroy, finliize, ...
}

static int thyme_connect(char *data, u32 len)
{
    int rc = 0;
    char* host = get_broker_ip();
    int port = get_broker_port();
    int keepalive = get_keepalive();
    char* userName = NULL;
    char* password = NULL;
    int enableServerCertAuth = get_cert_auth();
    int cleanSession = 1;
    char* clientID = "test_client";
    log_I("thyme_connect\n");

    mqtt_state(MQTT_CONNECTING);
    rc = mqtt_create(host, port, clientID);
    rc = mqtt_setcb(onConnected, onReconnect, onSubscribed, onDisconnected,
                onConnectionLost, onMessageDelivered, onMessageArrived);
    rc = mqtt_connect(keepalive, userName, password, enableServerCertAuth, cleanSession);
    log_I("thyme_connect rc[%d]\n", rc);
    return 0;
}

static int thyme_subscribe(char *data, u32 len)
{
    int rc;
    log_I("thyme_subscribe\n");

    char *topics[5];
    topics[0] = get_req_topic();
    topics[1] = get_resp_topic();
    topics[2] = get_noti_topic();
    topics[3] = get_noti_resp_topic();
    topics[4] = get_heartbeat_topic();

    rc = mqtt_subscribe_array(topics, 5, MQTT_QOS_2);
    if(rc != MQTT_SUCCESS)
    {
        put_event(AE_DISCON, NULL, 0);
    }

    return rc;
}

static int thyme_publish(char *data, u32 len)
{
    int rc;
    char payload[PAYLOAD_SZ] = {0,};
    char *topic = get_req_topic();
    log_I("thyme_publish\n");

    if(len >0)
    {
        memcpy(payload, data, len);
    }

    rc = mqtt_publish(topic, data, MQTT_QOS_0);
    return rc;
}

static int thyme_response(char *data, u32 len)
{
    int rc;
    char payload[PAYLOAD_SZ] = {0,};
    char *topic = get_noti_resp_topic();
    log_I("thyme_response\n");

    if(len >0)
    {
        memcpy(payload, data, len);
    }

    rc = mqtt_publish(topic, data, MQTT_QOS_0);
    return rc;
}

static int thyme_disconnect(char *data, u32 len)
{
    log_I("thyme_disconnect\n");
    //mqtt_disconnect();
    mqtt_distory();
    mqtt_state(MQTT_DISCONNECT);
    return 0;
}

static int thyme_ble_set(char *data, u32 len)
{
    log_I("thyme_ble_set\n");
    ble_tas_set(data, len);
    return 0;
}

static int thyme_ble_get(char *data, u32 len)
{
    char json_payload[256] = {0,};

    log_I("thyme_ble_get\n");

    thyme_publish(json_payload, len);

    return 0;
}

static int thyme_zigbee_set(char *data, u32 len)
{
    log_I("thyme_zigbee_set\n");
    zigbee_tas_set(data, len);
    return 0;
}

static int thyme_zigbee_get(char *data, u32 len)
{
    char json_payload[256] = {0,};

    log_I("thyme_zigbee_get\n");

    thyme_publish(json_payload, len);

    return 0;
}

static int thyme_zwave_set(char *data, u32 len)
{
    log_I("thyme_zwave_set\n");
    zwave_tas_set(data, len);
    return 0;
}

static int thyme_zwave_get(char *data, u32 len)
{
    char json_payload[256] = {0,};

    log_I("thyme_zwave_get\n");

    thyme_publish(json_payload, len);

    return 0;
}

typedef struct
{
    u32 cmd;                        // cmd
    int (*pf)(char *data, u32 len); // function
} t_cmd;

static const t_cmd cmd_tbl[] =
{
    { AE_INIT,          thyme_init          },
    { AE_CONNECT,       thyme_connect       },
    { AE_SUBSCRIBE,     thyme_subscribe     },
    { AE_PUBLISH,       thyme_publish       },
    { AE_RESPONSE,      thyme_response      },
    { AE_DISCON,        thyme_disconnect    },

    { BL_SET,           thyme_ble_set       },
    { BL_GET,           thyme_ble_get       },

    { ZI_SET,           thyme_zigbee_set    },
    { ZI_GET,           thyme_zigbee_get    },

    { ZW_SET,           thyme_zwave_set     },
    { ZW_GET,           thyme_zwave_get     },

    { TE_AE,            cli_test_publish_ae         },
    { TE_CNT,           cli_test_publish_cnt        },
    { TE_CIN,           cli_test_publish_cin        },
    { TE_SUB,           cli_test_publish_sub        },
    { TE_RESP,          cli_test_publish_resp       },
    { TE_BEAT,          cli_test_publish_heartbeat  },
};

int thyme_main_proc(void)
{
    int ret = PROC_IDLE;
    int i;
    event_q ev = handle_event();
    mqtt_state(MQTT_NONE);
    if(ev.event)
    {
        for(i = 0; i < sizeof(cmd_tbl)/sizeof(cmd_tbl[0]); i++)
        {
            if(cmd_tbl[i].cmd == ev.event)
            {
                (*(cmd_tbl[i].pf))(ev.data, ev.len);
                ret = PROC_CMD;
                break;
            }
        }
    }

    return ret;
}

