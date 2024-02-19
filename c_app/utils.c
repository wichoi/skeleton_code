#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "common.h"
#include "log_service.h"

int utils_init(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

int utils_deinit(void)
{
    int ret_val = RET_OK;
    log_i("%s\n", __func__);
    return ret_val;
}

