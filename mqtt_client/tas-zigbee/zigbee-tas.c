#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "../common.h"
#include "../debug/dbg-log.h"

#include "zigbee-tas.h"

static zigbeeTasCb* zbCb;

int zigbee_tas_init(zigbeeTasCb* cb)
{
    int rc = 0;
    log_I("zigbee_tas_init\n");

    zbCb = cb;
    //if(zbCb) zbCb("hello zigbee");

    return rc;
}

int zigbee_tas_set(char *data, u32 len)
{
    int rc = 0;
    log_I("zigbee_tas_set\n");
    return rc;
}

int zigbee_tas_get(char *data, u32 len)
{
    int rc = 0;
    log_I("zigbee_tas_get\n");
    if(zbCb) zbCb(data, len);
    return rc;
}


