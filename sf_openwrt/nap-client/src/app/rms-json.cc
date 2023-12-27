#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "json/cJSON.h"

#include "log.h"
#include "event-data.h"
#include "event.h"
#include "timer.h"
#include "rms-json.h"

rms_json::rms_json():
    _poll_obj()
{
}

rms_json::~rms_json()
{
}

int rms_json::init(void)
{
    int ret_val = RET_OK;
    log_d("rms_json::%s \n", __func__);

    // modem object
    _poll_obj.push_back(OBJ_MODEM_VER);
    _poll_obj.push_back(OBJ_MODEM_TECH);
    _poll_obj.push_back(OBJ_MODEM_BAND);
    _poll_obj.push_back(OBJ_MODEM_CH);
    _poll_obj.push_back(OBJ_MODEM_RSSI);
    _poll_obj.push_back(OBJ_MODEM_RSRP);
    _poll_obj.push_back(OBJ_MODEM_RSRQ);
    _poll_obj.push_back(OBJ_MODEM_SINR);
    _poll_obj.push_back(OBJ_MODEM_MCC);
    _poll_obj.push_back(OBJ_MODEM_MNC);
    _poll_obj.push_back(OBJ_MODEM_PDP);
    _poll_obj.push_back(OBJ_MODEM_SIM);
    _poll_obj.push_back(OBJ_MODEM_IMEI);
    _poll_obj.push_back(OBJ_MODEM_NUM);
    _poll_obj.push_back(OBJ_MODEM_APN);

    // internet object
    _poll_obj.push_back(OBJ_WAN1_TYPE);
    _poll_obj.push_back(OBJ_WAN1_IP);
    _poll_obj.push_back(OBJ_WAN1_SUBNET);
    _poll_obj.push_back(OBJ_WAN1_GW);
    _poll_obj.push_back(OBJ_WAN1_DNS1);
    _poll_obj.push_back(OBJ_WAN1_DNS2);
    _poll_obj.push_back(OBJ_WAN1_MAC);

    _poll_obj.push_back(OBJ_WAN2_ENABLE);
    _poll_obj.push_back(OBJ_WAN2_TYPE);
    _poll_obj.push_back(OBJ_WAN2_IP);
    _poll_obj.push_back(OBJ_WAN2_SUBNET);
    _poll_obj.push_back(OBJ_WAN2_GW);
    _poll_obj.push_back(OBJ_WAN2_DNS1);
    _poll_obj.push_back(OBJ_WAN2_DNS2);
    _poll_obj.push_back(OBJ_WAN2_MAC);

    _poll_obj.push_back(OBJ_LAN_IP);
    _poll_obj.push_back(OBJ_LAN_SUBNET);
    _poll_obj.push_back(OBJ_LAN_MAC);
    _poll_obj.push_back(OBJ_DHCP_ENABLE);
    _poll_obj.push_back(OBJ_DHCP_START);
    _poll_obj.push_back(OBJ_DHCP_END);
    _poll_obj.push_back(OBJ_DHCP_DNS1);
    _poll_obj.push_back(OBJ_DHCP_DNS2);
    _poll_obj.push_back(OBJ_DHCP_LEASE);

    // wifi object
    _poll_obj.push_back(OBJ_WIFI_ENABLE);
    _poll_obj.push_back(OBJ_WIFI_VERSION);
    _poll_obj.push_back(OBJ_WIFI_MODE);
    _poll_obj.push_back(OBJ_WIFI_SSID);
    _poll_obj.push_back(OBJ_WIFI_BSSID);
    _poll_obj.push_back(OBJ_WIFI_CHANNEL);

    // administration object
    _poll_obj.push_back(OBJ_SERIAL);
    _poll_obj.push_back(OBJ_MAC);
    _poll_obj.push_back(OBJ_MODEL);
    _poll_obj.push_back(OBJ_RMS_VER);
    _poll_obj.push_back(OBJ_HW_VER);
    _poll_obj.push_back(OBJ_SW_VER);
    _poll_obj.push_back(OBJ_RESET);
    _poll_obj.push_back(OBJ_REBOOT);
    _poll_obj.push_back(OBJ_WHITE_TRIG);
    _poll_obj.push_back(OBJ_UP_TRIG);
    _poll_obj.push_back(OBJ_UP_URL);
    _poll_obj.push_back(OBJ_UP_CHECKSUM);
    _poll_obj.push_back(OBJ_UP_DEV_KEY);
    _poll_obj.push_back(OBJ_UP_ST);
    _poll_obj.push_back(OBJ_UP_PERIOD);
    _poll_obj.push_back(OBJ_DEV_LOG);
    _poll_obj.push_back(OBJ_SYS_CALL);
    _poll_obj.push_back(OBJ_NTP);

    return ret_val;
}

int rms_json::deinit(void)
{
    log_d("%s\n", __func__);
    return RET_OK;
}

int rms_json::rms_create(list<json_param> &obj_list, string &json_body)
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
            log_i("%s\n%s\n", __func__, json_data);
            json_body = json_data;
            cJSON_free(json_data);
        }
        cJSON_Delete(json_obj);
    }

    return ret_val;
}

int rms_json::rms_parse(string &response)
{
    log_i("%s [%s]\n", __func__, response.c_str());
    int ret_val = RET_ERROR;
    if(!response.empty())
    {
        cJSON *root = cJSON_Parse(response.c_str());
        if(root != NULL)
        {
            cJSON *result_code = cJSON_GetObjectItemCaseSensitive(root, KEY_RESULT_CODE);
            if(result_code != NULL)
            {
                log_i("%s [%s] \n", KEY_RESULT_CODE, result_code->valueint == 1 ? "true" : "false");
                if(result_code->valueint == 1)
                {
                    ret_val = RET_OK;
                }
            }
        }
        cJSON_Delete(root);
    }
    return ret_val;
}
