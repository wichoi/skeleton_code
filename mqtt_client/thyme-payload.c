#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "common.h"
#include "debug/dbg-log.h"
#include "config/config.h"
#include "cJSON/cJSON.h"
#include "thyme-resource.h"

static void rand_str(char *dest, u8 length)
{
    char charset[] = "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0)
    {
        u8 index = random() & 0x3F;
        *dest++ = charset[index];
    }
    *dest = '\0';
}

int payload_ae(char* payload, int index, char* api)
{
    int rc = 0;
    char *ae_id = get_ae_id();
    char ty[64] = {0,};
    char to[64] = {0,};
    char rn[64] = {0,};
    char rqi[10];
    rand_str(rqi, 8);
    res_ae_read(index, ty, to, rn);

    log_V("payload_ae[%d]: ty[%s], to[%s], rn[%s]\n", index, ty, to, rn);
    log_V("ty[%s]\n", ty);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{\"op\":5,\"rqi\":\"-MHbyT6ow_\",\"to\":\"mqtt://220.85.233.146/Saproedu4?ct=json\",\"fr\":\"/Mobius2\",\"pc\":{\"m2m:sgn\":{\"sur\":\"Mobius/aproedu4/led/sub\",\"nev\":{\"rep\":{\"m2m:cin\":{\"rn\":\"4-20200602060714942\",\"ty\":\"4\",\"pi\":\"3-20200602052821729\",\"ri\":\"4-20200602060714943\",\"ct\":\"20200602T060714\",\"lt\":\"20200602T060714\",\"st\":\"2\",\"et\":\"20220602T060714\",\"cs\":\"1\",\"con\":\"1\",\"acpi\":[],\"lbl\":[],\"at\":[],\"aa\":[],\"subl\":[],\"cr\":\"SjRSYTDXgkX\"}},\"net\":3},\"rvi\":\"2a\"}}}");

/*
    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"1\","
    "\"to\":\"%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"ty\":\"%s\",",
    to, ae_id, rqi, ty);

    snprintf(payload, PAYLOAD_SZ - 1,
    "%s"
    "\"pc\":{"
    "\"m2m:ae\":{"
    "\"rn\":\"%s\","
    "\"api\":\"%s\","
    "\"rr\":\"true\""
    "}"
    "}"
    "}",
    payload, rn, api);
*/
    log_V("payload[%s]\n", payload);
    return rc;
#if 0
    int rc = 0;
    char *ae_id = get_ae_id();
    char ty[64] = {0,};
    char to[64] = {0,};
    char rn[64] = {0,};
    char rqi[10];
    rand_str(rqi, 8);
    res_ae_read(index, ty, to, rn);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"1\","
    "\"to\":\"%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"ty\":\"%s\","
    "\"pc\":{"
    "\"m2m:ae\":{"
    "\"rn\":\"%s\","
    "\"api\":\"%s\","
    "\"rr\":\"true\""
    "}"
    "}"
    "}",
    to, ae_id, rqi, ty, rn, api);

    log_V("payload[%s]\n", payload);
    return rc;
#endif
}


int payload_cnt(char* payload, int index)
{
    int rc = 0;
    char *ae_id = get_ae_id();
    char ty[64] = {0,};
    char to[64] = {0,};
    char rn[64] = {0,};
    char rqi[10];
    rand_str(rqi, 8);
    res_cnt_read(index, ty, to, rn);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"1\","
    "\"to\":\"%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"ty\":\"%s\",",
    to, ae_id, rqi, ty);

    snprintf(payload, PAYLOAD_SZ - 1,
    "%s"
    "\"pc\":{"
    "\"m2m:cnt\":{"
    "\"rn\":\"%s\""
    "}"
    "}"
    "}",
    payload, rn);

    log_V("payload[%s]\n", payload);
    return rc;
#if 0
    int rc = 0;
    char *ae_id = get_ae_id();
    char ty[64] = {0,};
    char to[64] = {0,};
    char rn[64] = {0,};
    char rqi[10];
    rand_str(rqi, 8);
    res_cnt_read(index, ty, to, rn);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"1\","
    "\"to\":\"%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"ty\":\"%s\","
    "\"pc\":{"
    "\"m2m:cnt\":{"
    "\"rn\":\"%s\""
    "}"
    "}"
    "}",
    to, ae_id, rqi, ty, rn);

    log_V("payload[%s]\n", payload);
    return rc;
#endif
}

int payload_sub_del(char* payload, int index)
{
    int rc = 0;
    char *ae_id = get_ae_id();
    char ty[64] = {0,};
    char to[64] = {0,};
    char rn[64] = {0,};
    char rqi[10];
    rand_str(rqi, 8);
    res_sub_read(index, ty, to, rn);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"4\","
    "\"to\":\"%s/%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"pc\":{"
    "}"
    "}",
    to, rn, ae_id, rqi);

    log_V("payload[%s]\n", payload);
    return rc;
}

int payload_sub_create(char* payload, int index)
{
    int rc = 0;
    char *ae_id = get_ae_id();
    char* broker_ip = get_broker_ip();
    char ty[64] = {0,};
    char to[64] = {0,};
    char rn[64] = {0,};
    char rqi[10];
    rand_str(rqi, 8);
    res_sub_read(index, ty, to, rn);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"1\","
    "\"to\":\"%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"ty\":\"%s\","
    "\"pc\":{"
    "\"m2m:sub\":{"
    "\"rn\":\"%s\","
    "\"enc\":{"
    "\"net\":[3]},"
    "\"nu\":"
    "[\"mqtt://%s:1883/%s?ct=json&rcn=9\"],"
    "\"nct\":\"2\""
    "}"
    "}"
    "}",
    to, ae_id , rqi, ty, rn, broker_ip, ae_id);

    log_V("payload[%s]\n", payload);
    return rc;
}

int payload_cin(char* payload, char* to, char* value)
{
    int rc = 0;
    char *ae_id = get_ae_id();
    char rqi[10];
    rand_str(rqi, 8);

    snprintf(payload, PAYLOAD_SZ - 1,
    "{"
    "\"op\":\"1\","
    "\"to\":\"%s?rcn=0\","
    "\"fr\":\"%s\","
    "\"rqi\":\"%s\","
    "\"ty\":\"4\","
    "\"pc\":{"
    "\"m2m:cin\":{"
    "\"con\":%s"
    "}"
    "}"
    "}",
    to, ae_id, rqi, value);

    log_V("payload[%s]\n", payload);
    return rc;
}


