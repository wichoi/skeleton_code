#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cJSON.h"

#include "log.h"
#include "event.h"
#include "json-manager.h"

json_manager::json_manager()
{
}

json_manager::~json_manager()
{
}

int json_manager::init(void)
{
    int ret_val = RET_OK;
    log_d("json_manager::%s \n", __func__);
    return ret_val;
}

int json_manager::deinit(void)
{
    log_d("%s\n", __func__);
    return RET_OK;
}

int json_manager::config_create(list<json_param> &obj_list, string &json_body)
{
    int ret_val = RET_ERROR;
    cJSON *json_obj = cJSON_CreateObject();
    if(json_obj != NULL)
    {
        while(obj_list.size() > 0)
        {
            json_param data = obj_list.front();
            obj_list.pop_front();
            if(data.type == json_param::TYPE_STR)
                cJSON_AddStringToObject(json_obj, data.key.c_str(), data.value_str.c_str());
            else if(data.type == json_param::TYPE_INT)
                cJSON_AddNumberToObject(json_obj, data.key.c_str(), data.value_int);
            else if(data.type == json_param::TYPE_NUM)
                cJSON_AddNumberToObject(json_obj, data.key.c_str(), data.value_num);

            ret_val = RET_OK;
        }

        char *json_data = NULL;
        json_data = cJSON_Print(json_obj);
        if(json_data != NULL)
        {
            log_v("%s\n%s\n", __func__, json_data);
            json_body = json_data;
            cJSON_free(json_data);
        }
        cJSON_Delete(json_obj);
    }
    return ret_val;
}

int json_manager::config_parse(string &src_data, list<json_param> &obj_list)
{
    log_i("%s\n%s\n", __func__, src_data.c_str());
    int ret_val = RET_ERROR;

    if(!src_data.empty())
    {
        cJSON *root = cJSON_Parse(src_data.c_str());
        if(root != NULL)
        {
            list<json_param>::iterator iter;
            for(iter = obj_list.begin(); iter != obj_list.end(); ++iter)
            {
                string str = iter->key;
                cJSON *result = cJSON_GetObjectItemCaseSensitive(root, str.c_str());
                if(result != NULL)
                {
                    if(iter->type == json_param::TYPE_STR)
                    {
                        if(cJSON_GetStringValue(result) != NULL)
                        {
                            log_d("%s [%s] \n", str.c_str(), result->valuestring);
                            iter->value_str = result->valuestring;
                        }
                    }
                    else if(iter->type == json_param::TYPE_INT)
                    {
                        log_d("%s [%d] \n", str.c_str(), result->valueint);
                        iter->value_int = result->valueint;
                    }
                    else if(iter->type == json_param::TYPE_NUM)
                    {
                        log_d("%s [%lf] \n", str.c_str(), result->valuedouble);
                        iter->value_num = result->valuedouble;
                    }
                }
            }

            ret_val = RET_OK;
        }
        cJSON_Delete(root);
    }

    return ret_val;
}

int json_manager::ota_metadata_dfd_parser(string &src, string &url, string &m2g2g, string &m4g2g)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, src.c_str());



    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::ota_metadata_mfd_parser(string &src, string &url, list<mdm_fw_list> &fw_list)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, src.c_str());



    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_body_auth(string &json_body, param_auth_t &param_data)
{
    int ret_val = RET_ERROR;

    log_i("%s\n", json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_sinfo(string &json_body, param_sinfo_t &param_data)
{
    int ret_val = RET_ERROR;


    log_i("%s\n", json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_sauth(string &json_body, param_sauth_t &param_data)
{
    int ret_val = RET_ERROR;

    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_boot(string &json_body, param_boot_t &param_data)
{
    int ret_val = RET_ERROR;


    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_event(string &json_body, param_event_t &param_data, int type)
{
    int ret_val = RET_ERROR;

    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_report(string &json_body, param_report_t &param_data)
{
    int ret_val = RET_ERROR;

    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_heartbeat(string &json_body, param_heartbeat_t &param_data)
{
    int ret_val = RET_ERROR;

    return ret_val;
}

int json_manager::cloud_body_getkey(string &json_body, param_getkey_t &param_data)
{
    int ret_val = RET_ERROR;

    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_keyupdated(string &json_body, param_keyupdated_t &param_data)
{
    int ret_val = RET_ERROR;

    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_getparam(string &json_body, param_getparam_t &param_data)
{
    int ret_val = RET_ERROR;

    return ret_val;
}

int json_manager::cloud_body_paramupdated(string &json_body, param_paramupdated_t &param_data)
{
    int ret_val = RET_ERROR;

    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_body_getfwup(string &json_body, param_getfwup_t &param_data)
{
    int ret_val = RET_ERROR;


    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}


int json_manager::cloud_body_fwup(string &json_body, param_fwup_t &param_data)
{
    int ret_val = RET_ERROR;


    log_i("%s\n%s\n", __func__, json_body.c_str());
    return ret_val;
}

int json_manager::cloud_response_auth(string &recv_body, result_auth_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());


    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_sauth(string &recv_body, result_sauth_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());


    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_boot(string &recv_body, result_boot_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());


    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_event(string &recv_body, result_event_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_report(string &recv_body, result_report_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_heartbeat(string &recv_body, result_heartbeat_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_getkey(string &recv_body, result_getkey_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_keyupdated(string &recv_body, result_keyupdated_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_getparam(string &recv_body, result_getparam_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_paramupdated(string &recv_body, result_paramupdated_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());


    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_getfwup(string &recv_body, result_getfwup_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int json_manager::cloud_response_fwup(string &recv_body, result_fwup_t &result)
{
    int ret_val = RET_ERROR;
    log_i("%s\n%s\n", __func__, recv_body.c_str());


    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

