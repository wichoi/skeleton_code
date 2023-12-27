#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json/cJSON.h"

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
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
            json_param data = move(obj_list.front());
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
                        if(strlen(result->valuestring) > 0)
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

