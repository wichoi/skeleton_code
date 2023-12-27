#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json/cJSON.h"

#include "log.h"
#include "auth-json.h"

auth_json::auth_json()
{
}

auth_json::~auth_json()
{
}

int auth_json::init(void)
{
    int ret_val = RET_OK;
    log_d("auth_json::%s \n", __func__);
    return ret_val;
}

int auth_json::deinit(void)
{
    log_d("%s\n", __func__);
    return RET_OK;
}

int auth_json::token_create(string &request, string &iss, string &sub, string &aud, string &jti, string &exp)
{
    int ret_val = RET_ERROR;
    cJSON *json_obj = cJSON_CreateObject();
    if(json_obj != NULL)
    {
        cJSON_Delete(json_obj);
    }
    return ret_val;
}

int auth_json::token_parse(string &response, string &token, int &expired, string &type, string &error, string &description, string &message)
{
    log_d("%s [%s]\n", __func__, response.c_str());
    int ret_val = RET_ERROR;
    return ret_val;
}

