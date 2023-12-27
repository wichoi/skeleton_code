#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#include "log.h"
#include "utils.h"
#include "config-handler.h"
#include "config-manager.h"

config_handler::config_handler():
    _json_mgr()
{
}

config_handler::~config_handler()
{
}

int config_handler::init()
{
    int ret_val = RET_OK;
    log_d("config_handler::%s \n", __func__);
    _json_mgr.init();
    read_config(true);
    read_nvram();
    field_test();
    return ret_val;
}

int config_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _json_mgr.deinit();
    return RET_OK;
}

int config_handler::object_list(list<json_param> &obj_list)
{
    string current_time;
    utils::read_timestamp(current_time);
    string sw_ver;
    utils::read_version(sw_ver);
    obj_list.push_back(json_param(CONF_WRITE_TIME, current_time));
    obj_list.push_back(json_param(CONF_VER_MAJOR, (int)config_manager::instance()->get_con_ver_major()));
    obj_list.push_back(json_param(CONF_VER_MINOR, (int)config_manager::instance()->get_con_ver_minor()));
    obj_list.push_back(json_param(CONF_SW_VER, sw_ver));
    obj_list.push_back(json_param(CONF_NORDIC_VER, config_manager::instance()->get_nordic_version()));

    return RET_OK;
}

int config_handler::read_config(bool ver_check)
{
    int ret_val = RET_ERROR;
    FILE* fp = NULL;
    string src_data = "";
    char buf[256] = {0,};
    list<json_param> obj_list;
    list<json_param>::iterator iter;
    u32 ver_major = 0, ver_minor = 0;

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
        if(flock(fileno(fp), LOCK_EX) < 0)
        {
            log_i("%s file is locked \n", __func__);
            fclose(fp);
            goto free_all;
        }

        while(!feof(fp))
        {
            if(fgets(buf, sizeof(buf), fp) != nullptr)
            {
                src_data.append(buf);
            }
        }

        flock(fileno(fp), LOCK_UN);
    }
    else
    {
        log_w("%s %s open fail !!!\n", __func__, CONFIG_FILE);
        goto free_all;
    }

    object_list(obj_list);
    if(_json_mgr.config_parse(src_data, obj_list) != RET_OK)
    {
        log_w("%s config file broken !!!\n", __func__);
        goto free_all;
    }

    for(iter = obj_list.begin(); iter != obj_list.end(); ++iter)
    {
        if(iter->key.compare(CONF_VER_MAJOR) == 0)
        {
            ver_major = (u32)iter->value_int;
        }
        else if(iter->key.compare(CONF_VER_MINOR) == 0)
        {
            ver_minor = (u32)iter->value_int;
        }
    }

    if(!(ver_major == config_manager::instance()->get_con_ver_major() &&
       ver_minor == config_manager::instance()->get_con_ver_minor()))
    {
        log_w("%s config version mismatch [%d.%d] [%d.%d] \n",
                    __func__, ver_major, ver_minor,
                    config_manager::instance()->get_con_ver_major(),
                    config_manager::instance()->get_con_ver_minor());
        goto free_all;
    }

    for(iter = obj_list.begin(); iter != obj_list.end(); ++iter)
    {
        if(iter->key.compare(CONF_URL_AUTH) == 0)
        {
            config_manager::instance()->set_auth_url(iter->value_str);
        }
        else if(iter->key.compare(CONF_URL_TOKEN) == 0)
        {
            config_manager::instance()->set_token_url(iter->value_str);
        }
    }

    ret_val = RET_OK;

free_all:
    if(fp != NULL) fclose(fp);

    log_i("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int config_handler::write_config(void)
{
    int ret_val = RET_ERROR;

    list<json_param> obj_list;
    object_list(obj_list);

    string json_body;
    _json_mgr.config_create(obj_list, json_body);

    if(access(CONFIG_FILE_BAK, F_OK) == 0)
    {
        remove(CONFIG_FILE_BAK);
    }
    rename(CONFIG_FILE, CONFIG_FILE_BAK);

    FILE* fp = fopen(CONFIG_FILE, "wt");
    if (fp == NULL)
    {
        log_i("%s %s open fail !!!\n", __func__, CONFIG_FILE);
        goto free_all;
    }

    if(flock(fileno(fp), LOCK_EX) < 0)
    {
        log_i("%s file is locked \n", __func__);
        goto free_all;
    }

    fwrite(json_body.c_str(), 1, json_body.length(), fp);
    flock(fileno(fp), LOCK_UN);

    ret_val = RET_OK;

free_all:
    if(fp != NULL) fclose(fp);

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int config_handler::delete_config(void)
{
    log_i("%s \n", __func__);
    if(access(CONFIG_FILE, F_OK) == 0)
    {
        remove(CONFIG_FILE);
    }

    if(access(CONFIG_FILE_BAK, F_OK) == 0)
    {
        remove(CONFIG_FILE_BAK);
    }

    return RET_OK;
}

int config_handler::read_nvram(void)
{
    string key = "serial";
    string value = "";
    utils::nvram_uboot_get(key, value);
    log_i("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    if(value.length() > 0)
    {
        config_manager::instance()->set_serial(value);
    }

    return RET_OK;
}

int config_handler::field_test(void)
{
    // 0.7.5 version is overwrite url one time
    string version = "";
    utils::read_version(version);
    if(version.compare("0.7.5") == 0)
    {
        u32 value = config_manager::instance()->get_field_test();
        if(value == 0 || value == 1 || value == 2)
        {

        }
    }

    return RET_OK;
}

