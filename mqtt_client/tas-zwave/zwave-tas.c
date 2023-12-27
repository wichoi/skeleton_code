#include <stdio.h>
#include <string.h>
#include <stdbool.h> 
#include <stdlib.h>

#include "../common.h"
#include "../debug/dbg-log.h"

#include "zwave-tas.h"

static zwaveTasCb* zwCb;

int zwave_tas_init(zwaveTasCb* cb)
{
    int rc = 0;
    log_I("zwave_tas_init\n");

    zwCb = cb;
    //if(zwCb) zwCb("hello zigbee");

    return rc;
}

int zwave_tas_set(char *data, u32 len)
{
    int rc = 0;
    log_I("zwave_tas_set\n");
    return rc;
}

int zwave_tas_get(char *data, u32 len)
{
    int rc = 0;
    log_I("zwave_tas_get\n");
    if(zwCb) zwCb(data, len);
    return rc;
}



