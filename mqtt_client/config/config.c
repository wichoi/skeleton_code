#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "../common.h"
#include "../debug/dbg-log.h"
#include "config.h"

// Information of CSE as Mobius with MQTT
static char* broker_ip = NULL;
static u16 broker_port = 0;
static int keepalive = 0;
static int cert_auth = 0;

static char* ae_name = NULL; // air1";
static char* ae_id = NULL; // "Sair1"; // + AE_NAME;
static char* cse_id = NULL; // "Mobius2";
static char* cb_name = NULL;

static char req_topic[48] = {0x0,};
static char resp_topic[48] = {0x0,};
static char noti_topic[48] = {0x0,};
static char noti_resp_topic[48] = {0x0,};
static char heartbeat_topic[48] = {0x0,};

int init_config(void)
{
    log_V("init_config\n");

    broker_ip = "192.168.10.171"; // 127.0.0.1"; // "220.85.233.146";
    //broker_ip = "220.85.233.146";
    broker_port = 1883;
    keepalive = 120;
    cert_auth = 0;
    
    ae_name = "AT-GW200";
    ae_id = "SAT-GW200";
    cse_id = "Mobius";
    cb_name = "Mobius";

    snprintf(req_topic, sizeof(req_topic) - 1, "/oneM2M/req/%s/%s/json", ae_id, cse_id);
    //snprintf(resp_topic, sizeof(resp_topic) - 1, "/oneM2M/resp/%s/json", AE_ID);
    //snprintf(noti_topic, sizeof(noti_topic) - 1, "/oneM2M/req/Mobius/%s/json", AE_ID);
    snprintf(resp_topic, sizeof(resp_topic) - 1, "/oneM2M/resp/%s/%s/json", ae_id, cse_id);
    snprintf(noti_topic, sizeof(noti_topic) - 1, "/oneM2M/req/%s/%s/json", cse_id, ae_id);
    snprintf(noti_resp_topic, sizeof(noti_resp_topic) - 1, "/oneM2M/resp/%s/%s/json", cse_id, ae_id);
    snprintf(heartbeat_topic, sizeof(heartbeat_topic) - 1, "/nCube/heartbeat");

    return 0;
}

char* get_broker_ip(void)
{
    log_V("get_broker_ip : %s\n", broker_ip);
    return broker_ip;
}

u16 get_broker_port(void)
{
    log_V("get_broker_port : %d\n", broker_port);
    return broker_port;
}

int get_keepalive(void)
{
    log_V("get_keepalive : %d\n", keepalive);
    return keepalive;
}

int get_cert_auth(void)
{
    log_V("get_cert_auth : %d\n", cert_auth);
    return cert_auth;
}

char* get_ae_name(void)
{
    log_V("get_ae_name : %s\n", ae_name);
    return ae_name;
}

char* get_ae_id(void)
{
    log_V("get_ae_id : %s\n", ae_id);
    return ae_id;
}

char* get_cse_id(void)
{
    log_V("get_cse_id : %s\n", cse_id);
    return cse_id;
}

char* get_cb_name(void)
{
    log_V("get_cb_name : %s\n", cb_name);
    return cb_name;
}

char* get_req_topic(void)
{
    log_V("get_req_topic : %s\n", req_topic);
    return req_topic;
}

char* get_resp_topic(void)
{
    log_V("get_resp_topic : %s\n", resp_topic);
    return resp_topic;
}

char* get_noti_topic(void)
{
    log_V("get_noti_topic : %s\n", noti_topic);
    return noti_topic;
}

char* get_noti_resp_topic(void)
{
    log_V("get_noti_resp_topic : %s\n", noti_resp_topic);
    return noti_resp_topic;
}

char* get_heartbeat_topic(void)
{
    log_V("get_heartbeat_topic : %s\n", heartbeat_topic);
    return heartbeat_topic;
}




