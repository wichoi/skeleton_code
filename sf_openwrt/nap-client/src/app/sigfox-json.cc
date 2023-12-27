#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json/cJSON.h"

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "sigfox-json.h"

sigfox_json::sigfox_json()
{
}

sigfox_json::~sigfox_json()
{
}

int sigfox_json::init(void)
{
    int ret_val = RET_OK;
    log_d("sigfox_json::%s \n", __func__);
    return ret_val;
}

int sigfox_json::deinit(void)
{
    log_d("%s\n", __func__);
    return RET_OK;
}
