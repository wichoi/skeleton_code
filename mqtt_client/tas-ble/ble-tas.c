#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "../common.h"
#include "../debug/dbg-log.h"
#include "ble-tas.h"

static bleTasCb* bleCb;

int ble_tas_init(bleTasCb* cb)
{
    int rc = 0;
    log_I("ble_tas_init\n");

    bleCb = cb;
    //if(bleCb) bleCb("hello ble");

    return rc;
}

int ble_tas_set(char *data, u32 len)
{
    int rc = 0;
    log_I("ble_tas_set\n");
    return rc;
}

int ble_tas_get(char *data, u32 len)
{
    int rc = 0;
    log_I("ble_tas_get\n");
    if(bleCb) bleCb(data, len);
    return rc;
}

