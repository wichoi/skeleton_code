#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "log_service.h"
#include "config_manager.h"


static char _version[16] = {0,};
static char _model_name[16] = {0,};
static int _main_state = 0;
static int _secc_state = 0;

int config_default(void)
{
    log_i("%s\n", __func__);

    strcpy(_version, "0.0.1");
    strcpy(_model_name, "AT-EV320");
    _main_state = 0;
    _secc_state = 0;

    return RET_OK;
}

int config_init(void)
{
    log_i("%s\n", __func__);
    config_default();
    return RET_OK;
}

int config_deinit(void)
{
    log_i("%s\n", __func__);
    return RET_OK;
}

int config_read(void)
{
    log_i("%s\n", __func__);
    // todo read config file
    return RET_OK;
}

int config_write(void)
{
    log_i("%s\n", __func__);
    // todo write config file
    return RET_OK;
}

int config_update(void)
{
    log_i("%s\n", __func__);
    // todo update config file
    return RET_OK;
}

const char* config_version_get(void)
{
    log_i("%s[%s]\n", __func__, _version);
    return _version;
}

int config_version_set(char *value)
{
    snprintf(_version, sizeof(_version) - 1, "%s", value);
    log_i("%s[%s]\n", __func__, _version);
    return RET_OK;
}

const char* config_model_name_get(void)
{
    log_i("%s[%s]\n", __func__, _model_name);
    return _model_name;
}

int config_model_name_set(char *value)
{
    snprintf(_model_name, sizeof(_model_name) - 1, "%s", value);
    log_i("%s[%s]\n", __func__, _model_name);
    return RET_OK;
}

int config_main_state_get(void)
{
    log_d("%s[%d]\n", __func__, _main_state);
    return _main_state;
}

int config_main_state_set(int value)
{
    _main_state = value;
    log_d("%s[%d]\n", __func__, _main_state);
    return RET_OK;
}

int config_secc_state_get(void)
{
    log_d("%s[%d]\n", __func__, _secc_state);
    return _secc_state;
}

int config_secc_state_set(int value)
{
    _secc_state = value;
    log_d("%s[%d]\n", __func__, _secc_state);
    return RET_OK;
}

