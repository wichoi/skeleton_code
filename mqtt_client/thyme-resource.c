#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "common.h"
#include "debug/dbg-log.h"
#include "config/config.h"

#define AE_COUNT 1
#define CNT_COUNT 10
#define SUB_COUNT 5

typedef struct _resource_t {
  char ty[64];
  char to[64];
  char rn[64];
  int status;
} resource_t;

static resource_t ae[AE_COUNT];
static int ae_count;
static resource_t cnt[CNT_COUNT];
static int cnt_count;
static resource_t sub[SUB_COUNT];
static int sub_count;

void res_ae_add(char* ty, char* to, char* rn)
{
    snprintf(ae[ae_count].ty, sizeof(ae[ae_count].ty) - 1, "%s", ty);
    snprintf(ae[ae_count].to, sizeof(ae[ae_count].to) - 1, "%s", to);
    snprintf(ae[ae_count].rn, sizeof(ae[ae_count].rn) - 1, "%s", rn);
    ae[ae_count].status = 0;
    log_V("res_ae_add[%d]: ty[%s], to[%s], rn[%s]\n",
            ae_count, ae[ae_count].ty, ae[ae_count].to, ae[ae_count].rn);
    ae_count++;
}

void res_cnt_add(char* ty, char* to, char* rn)
{
    snprintf(cnt[ae_count].ty, sizeof(cnt[ae_count].ty) - 1, "%s", ty);
    snprintf(cnt[ae_count].to, sizeof(cnt[ae_count].to) - 1, "%s", to);
    snprintf(cnt[ae_count].rn, sizeof(cnt[ae_count].rn) - 1, "%s", rn);
    cnt[cnt_count].status = 0;
    cnt_count++;
}

void res_sub_add(char* ty, char* to, char* rn)
{
    snprintf(sub[ae_count].ty, sizeof(sub[ae_count].ty) - 1, "%s", ty);
    snprintf(sub[ae_count].to, sizeof(sub[ae_count].to) - 1, "%s", to);
    snprintf(sub[ae_count].rn, sizeof(sub[ae_count].rn) - 1, "%s", rn);
    sub[sub_count].status = 0;
    sub_count++;
}

//static void res_ae_del();
//static void res_cnt_del();
//static void res_sub_del();

//static void res_ae_update();
//static void res_cnt_update();
//static void res_sub_update();

int res_ae_read(int ix, char* ty, char* to, char* rn)
{
    int rc = 0;
    memcpy(ty, ae[ix].ty, sizeof(ae[ix].ty));
    memcpy(to, ae[ix].to, sizeof(ae[ix].to));
    memcpy(rn, ae[ix].rn, sizeof(ae[ix].rn));
    return rc;
}

int res_cnt_read(int ix, char* ty, char* to, char* rn)
{
    int rc = 0;
    memcpy(ty, cnt[ix].ty, sizeof(cnt[ix].ty));
    memcpy(to, cnt[ix].to, sizeof(cnt[ix].to));
    memcpy(rn, cnt[ix].rn, sizeof(cnt[ix].rn));
    return rc;
}

int res_sub_read(int ix, char* ty, char* to, char* rn)
{
    int rc = 0;
    memcpy(ty, sub[ix].ty, sizeof(sub[ix].ty));
    memcpy(to, sub[ix].to, sizeof(sub[ix].to));
    memcpy(rn, sub[ix].rn, sizeof(sub[ix].rn));
    return rc;
}

void res_init(void)
{
    char *ae_name = get_ae_name();
    char *cb_name = get_cb_name();
    log_V("res_init\n");

    ae_count = 0;
    cnt_count = 0;
    sub_count = 0;

    char buf[64] ={0,};
    snprintf(buf, sizeof(buf) - 1, "/%s", cb_name);
    res_ae_add("2", buf, ae_name);     // AE resource

    snprintf(buf, sizeof(buf) - 1, "/%s/%s", cb_name, ae_name);
    res_cnt_add("3", buf, "ble");       // Container resource
    res_cnt_add("3", buf, "zigbee");    // Container resource
    res_cnt_add("3", buf, "z-wave");    // Container resource
    res_cnt_add("3", buf, "common");    // Container resource

    snprintf(buf, sizeof(buf) - 1, "/%s/%s/ble", cb_name, ae_name);
    res_sub_add("23", buf, "sub");      // Subscription resource

    snprintf(buf, sizeof(buf) - 1, "/%s/%s/zigbee", cb_name, ae_name);
    res_sub_add("23", buf, "sub");      // Subscription resource
}

