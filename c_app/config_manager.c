#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>

#include "jRead.h"
#include "jWrite.h"

#include "common.h"
#include "log_service.h"
#include "config_manager.h"

#define CONFIG_FILE             "config.conf"
#define CONFIG_FILE_BAK         "config.conf.bak"

#define KEY_VERSION             "version"
#define KEY_MODEL_NAME          "model"
#define KEY_MAIN_STATE          "main_st"
#define KEY_SECC_STATE          "secc_st"

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
    config_read();
    return RET_OK;
}

int config_deinit(void)
{
    log_i("%s\n", __func__);
    config_write();
    return RET_OK;
}

int config_read(void)
{
    FILE* fp = NULL;
    char jbuff[4096] = {0,};
    char buf[32] = {0};

    if(access(CONFIG_FILE, F_OK) == 0)
    {
        fp = fopen(CONFIG_FILE, "rt");
        log_i("%s %s\n", __func__, CONFIG_FILE);
    }
    else if(access(CONFIG_FILE_BAK, F_OK) == 0)
    {
        fp = fopen(CONFIG_FILE_BAK, "rt");
        log_i("%s %s\n", __func__, CONFIG_FILE_BAK);
    }

    if(fp != NULL)
    {
        fread(jbuff, 4096, 1, fp);
        fclose(fp);
    }

    jRead_string(jbuff, "{'" KEY_VERSION "'" , buf, sizeof(buf), NULL);
    if(strlen(buf) > 0) snprintf(_version, sizeof(_version) - 1, "%s", buf);
    jRead_string(jbuff, "{'" KEY_MODEL_NAME "'", buf, sizeof(buf), NULL);
    if(strlen(buf) > 0) snprintf(_model_name, sizeof(_model_name) - 1, "%s", buf);
    _main_state = jRead_int(jbuff, "{'" KEY_MODEL_NAME "'", NULL);
    _secc_state = jRead_int(jbuff, "{'" KEY_SECC_STATE "'", NULL);
#if 0
    jRead_int   (jbuff, "{'body'{'key1'", NULL);
    jRead_string(jbuff, "{'body'{'key2'", buf, sizeof(buf), NULL);
    jRead_int   (jbuff, "{'body'{'key3'", NULL);
#endif

    log_i("%s\n%s\n", __func__, jbuff);
    return RET_OK;
}

int config_write(void)
{
    struct jWriteControl jwc;
    char jbuff[4096] = {0,};

    jwOpen(&jwc, jbuff, 4096, JW_OBJECT, JW_PRETTY);

    jwObj_string(&jwc, KEY_VERSION,     _version);
    jwObj_string(&jwc, KEY_MODEL_NAME,  _model_name);
    jwObj_int   (&jwc, KEY_MAIN_STATE,  _main_state);
    jwObj_int   (&jwc, KEY_SECC_STATE,  _secc_state);
#if 0
    jwObj_object(&jwc, "body");
        jwObj_int       (&jwc, "key1",       0);
        jwObj_string    (&jwc, "key2",       "value2");
        jwObj_bool      (&jwc, "key3",       1);
    jwEnd(&jwc);
#endif
    int err = jwClose(&jwc);
    if(err != JWRITE_OK)
    {
        log_e("%s err[%s][%d]\n", __func__, jwErrorToString(err), jwErrorPos(&jwc));
    }

    if(access(CONFIG_FILE_BAK, F_OK) == 0)
    {
        remove(CONFIG_FILE_BAK);
    }
    rename(CONFIG_FILE, CONFIG_FILE_BAK);

    FILE* fp = fopen(CONFIG_FILE, "wt");
    if (fp == NULL)
    {
        log_i("%s %s open fail !!!\n", __func__, CONFIG_FILE);
    }
    else
    {
        fwrite(jbuff, 1, strlen(jbuff), fp);
        fclose(fp);
    }

    log_i("%s\n%s\n", __func__, jbuff);
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

