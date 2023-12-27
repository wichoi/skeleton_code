#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#include "log.h"
#include "utils.h"
#include "config-handler.h"
#include "config-manager.h"

#ifdef LINUX_PC_APP
  #define CONFIG_FILE           "modem.conf"
  #define CONFIG_FILE_BAK       "modem.conf.bak"

  #define CONFIG_NAP            "nap-client.conf"
  #define CONFIG_NAP_BAK        "nap-client.conf.bak"
#else
  #define CONFIG_FILE           "/data/modem.conf"
  #define CONFIG_FILE_BAK       "/data/modem.conf.bak"

  #define CONFIG_NAP            "/data/nap-client.conf"
  #define CONFIG_NAP_BAK        "/data/nap-client.conf.bak"
#endif

// config write time
#define CONF_WRITE_TIME         "conf_time"

// config version
#define CONF_VERSION            "conf_version"

// at-handle
#define CONF_DEV_PATH           "dev_path"
#define CONF_AT_BUF_SZ          "at_buf_size"
#define CONF_AT_TIMEOUT         "at_timeout"

// modem proc
#define CONF_MODEM_ENABLE       "mod_enable"
#define CONF_MODEM_TIMEOUT      "mod_timeout"
#define CONF_MODEM_IMEI         "imei"
#define CONF_MODEM_REVISION     "revision"
#define CONF_MODEM_VERSION      "version"
#define CONF_MODEM_IMSI         "imsi"
#define CONF_MODEM_CPIN         "cpin"
#define CONF_MODEM_QCCID        "qccid"
#define CONF_MODEM_OPERATOR     "net_operator"
#define CONF_MODEM_TECH         "access_technology"
#define CONF_MODEM_OP_NUM       "operator_number"
#define CONF_MODEM_BAND         "band"
#define CONF_MODEM_CHANNEL      "channel"
#define CONF_MODEM_SYS_MODE     "sys_mode"
#define CONF_MODEM_RSSI         "rssi"
#define CONF_MODEM_RSRP         "rsrp"
#define CONF_MODEM_SINR         "sinr"
#define CONF_MODEM_RSRQ         "rsrq"
#define CONF_MODEM_FNN          "full_name"
#define CONF_MODEM_SNN          "short_name"
#define CONF_MODEM_SPN          "provider_name"
#define CONF_MODEM_RPLWN        "plmn"
#define CONF_MODEM_PDP_TYPE     "pdp_type"
#define CONF_MODEM_APN          "apn"
#define CONF_MODEM_PDP_ADDR     "pdp_addr"
#define CONF_MODEM_CCLK         "cclk"
#define CONF_MODEM_USIM         "usim"
#define CONF_MODEM_NUMBER       "number"

#define CONF_LONGITUDE          "longitude"
#define CONF_LATITUDE           "latitude"

//#define CONF_FIELD_TEST         "field_test"

// nap-client.conf
#define CONF_GNSS_ENABLE        "gnss_enable"
#define CONF_GNSS_CONFIG        "gnss_config"
#define CONF_GNSS_INTERVAL      "gnss_interval"
#define CONF_GNSS_FIX_TIMEOUT   "gnss_fix_timeout"
#define CONF_GNSS_FIX_INTERVAL  "gnss_fix_interval"
#define CONF_GNSS_RETRY_CNT     "gnss_retry_cnt"

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
    //field_test();
    nap_read_config();
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
    obj_list.push_back(json_param(CONF_WRITE_TIME, current_time));

    string current_version;
    config_manager::instance()->get_sw_version(current_version);
    obj_list.push_back(json_param(CONF_VERSION, current_version));

    // at-handle
    obj_list.push_back(json_param(CONF_DEV_PATH, config_manager::instance()->get_at_dev_path()));
    obj_list.push_back(json_param(CONF_AT_BUF_SZ, (int)config_manager::instance()->get_at_buf_sz()));
    obj_list.push_back(json_param(CONF_AT_TIMEOUT, (int)config_manager::instance()->get_at_timeout()));

    // modem
    obj_list.push_back(json_param(CONF_MODEM_ENABLE, (int)config_manager::instance()->get_mod_enable()));
    obj_list.push_back(json_param(CONF_MODEM_TIMEOUT, (int)config_manager::instance()->get_mod_timeout()));
    obj_list.push_back(json_param(CONF_MODEM_IMEI, config_manager::instance()->get_imei()));
    obj_list.push_back(json_param(CONF_MODEM_REVISION, config_manager::instance()->get_revision()));
    obj_list.push_back(json_param(CONF_MODEM_VERSION, config_manager::instance()->get_version()));
    obj_list.push_back(json_param(CONF_MODEM_IMSI, config_manager::instance()->get_imsi()));
    obj_list.push_back(json_param(CONF_MODEM_CPIN, config_manager::instance()->get_cpin()));
    obj_list.push_back(json_param(CONF_MODEM_QCCID, config_manager::instance()->get_qccid()));
    obj_list.push_back(json_param(CONF_MODEM_OPERATOR, config_manager::instance()->get_net_operator()));
    obj_list.push_back(json_param(CONF_MODEM_TECH, config_manager::instance()->get_access_technology()));
    obj_list.push_back(json_param(CONF_MODEM_OP_NUM, config_manager::instance()->get_operator_number()));
    obj_list.push_back(json_param(CONF_MODEM_BAND, config_manager::instance()->get_band()));
    obj_list.push_back(json_param(CONF_MODEM_CHANNEL, (int)config_manager::instance()->get_channel()));
    obj_list.push_back(json_param(CONF_MODEM_SYS_MODE, config_manager::instance()->get_sys_mode()));
    obj_list.push_back(json_param(CONF_MODEM_RSSI, (int)config_manager::instance()->get_rssi()));
    obj_list.push_back(json_param(CONF_MODEM_RSRP, (int)config_manager::instance()->get_rsrp()));
    obj_list.push_back(json_param(CONF_MODEM_SINR, (int)config_manager::instance()->get_sinr()));
    obj_list.push_back(json_param(CONF_MODEM_RSRQ, (int)config_manager::instance()->get_rsrq()));
    obj_list.push_back(json_param(CONF_MODEM_FNN, config_manager::instance()->get_fnn()));
    obj_list.push_back(json_param(CONF_MODEM_SNN, config_manager::instance()->get_snn()));
    obj_list.push_back(json_param(CONF_MODEM_SPN, config_manager::instance()->get_spn()));
    obj_list.push_back(json_param(CONF_MODEM_RPLWN, config_manager::instance()->get_rplwn()));
    obj_list.push_back(json_param(CONF_MODEM_PDP_TYPE, config_manager::instance()->get_pdp_type()));
    obj_list.push_back(json_param(CONF_MODEM_APN, config_manager::instance()->get_apn()));
    obj_list.push_back(json_param(CONF_MODEM_PDP_ADDR, config_manager::instance()->get_pdp_addr()));
    obj_list.push_back(json_param(CONF_MODEM_CCLK, config_manager::instance()->get_cclk()));
    obj_list.push_back(json_param(CONF_MODEM_USIM, config_manager::instance()->get_usim()));
    obj_list.push_back(json_param(CONF_MODEM_NUMBER, config_manager::instance()->get_number()));
    obj_list.push_back(json_param(CONF_LONGITUDE, config_manager::instance()->get_longitude()));
    obj_list.push_back(json_param(CONF_LATITUDE, config_manager::instance()->get_latitude()));
    //obj_list.push_back(json_param(CONF_FIELD_TEST, (int)config_manager::instance()->get_field_test()));

    // nap-client.conf
    //obj_list.push_back(json_param(CONF_GNSS_ENABLE, (int)config_manager::instance()->get_gnss_enable()));
    //obj_list.push_back(json_param(CONF_GNSS_CONFIG, (int)config_manager::instance()->get_gnss_config()));
    //obj_list.push_back(json_param(CONF_GNSS_INTERVAL, (int)config_manager::instance()->get_gnss_interval()));
    //obj_list.push_back(json_param(CONF_GNSS_FIX_TIMEOUT, (int)config_manager::instance()->get_gnss_fix_timeout()));
    //obj_list.push_back(json_param(CONF_GNSS_FIX_INTERVAL, (int)config_manager::instance()->get_gnss_fix_interval()));
    //obj_list.push_back(json_param(CONF_GNSS_RETRY_CNT, (int)config_manager::instance()->get_gnss_retry_cnt()));

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
    _json_mgr.config_parse(src_data, obj_list);

    for(iter = obj_list.begin(); iter != obj_list.end(); ++iter)
    {
        if(iter->key.compare(CONF_VERSION) == 0)
        {
            string str_ver = "";
            config_manager::instance()->get_sw_version(str_ver);
            if(str_ver.compare(iter->value_str) != 0)
            {
                log_w("%s config version mismatch [%s] [%s] \n",
                            __func__, str_ver.c_str(), iter->value_str.c_str());
                goto free_all;
            }
        }
    }

    for(iter = obj_list.begin(); iter != obj_list.end(); ++iter)
    {
        if(iter->key.compare(CONF_DEV_PATH) == 0)
        {
            config_manager::instance()->set_at_dev_path(iter->value_str);
        }
        else if(iter->key.compare(CONF_AT_BUF_SZ) == 0)
        {
            config_manager::instance()->set_at_buf_sz((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_AT_TIMEOUT) == 0)
        {
            config_manager::instance()->set_at_timeout((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_ENABLE) == 0)
        {
            config_manager::instance()->set_mod_enable((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_TIMEOUT) == 0)
        {
            config_manager::instance()->set_mod_timeout((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_IMEI) == 0)
        {
            config_manager::instance()->set_imei(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_REVISION) == 0)
        {
            config_manager::instance()->set_revison(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_VERSION) == 0)
        {
            config_manager::instance()->set_version(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_IMSI) == 0)
        {
            config_manager::instance()->set_imsi(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_CPIN) == 0)
        {
            config_manager::instance()->set_cpin(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_QCCID) == 0)
        {
            config_manager::instance()->set_qccid(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_OPERATOR) == 0)
        {
            config_manager::instance()->set_net_operator(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_TECH) == 0)
        {
            config_manager::instance()->set_access_technology(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_OP_NUM) == 0)
        {
            config_manager::instance()->set_operator_number(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_BAND) == 0)
        {
            config_manager::instance()->set_band(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_CHANNEL) == 0)
        {
            config_manager::instance()->set_channel((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_SYS_MODE) == 0)
        {
            config_manager::instance()->set_sys_mode(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_RSSI) == 0)
        {
            config_manager::instance()->set_rssi((s32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_RSRP) == 0)
        {
            config_manager::instance()->set_rsrp((s32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_SINR) == 0)
        {
            config_manager::instance()->set_sinr((s32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_RSRQ) == 0)
        {
            config_manager::instance()->set_rsrq((s32)iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_FNN) == 0)
        {
            config_manager::instance()->set_fnn(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_SNN) == 0)
        {
            config_manager::instance()->set_snn(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_SPN) == 0)
        {
            config_manager::instance()->set_spn(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_RPLWN) == 0)
        {
            config_manager::instance()->set_rplwn(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_PDP_TYPE) == 0)
        {
            config_manager::instance()->set_pdp_type(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_APN) == 0)
        {
            config_manager::instance()->set_apn(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_PDP_ADDR) == 0)
        {
            config_manager::instance()->set_pdp_addr(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_CCLK) == 0)
        {
            config_manager::instance()->set_cclk(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_USIM) == 0)
        {
            config_manager::instance()->set_usim(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_NUMBER) == 0)
        {
            config_manager::instance()->set_number(iter->value_str);
        }
        else if(iter->key.compare(CONF_LONGITUDE) == 0)
        {
            config_manager::instance()->set_longitude(iter->value_fl);
        }
        else if(iter->key.compare(CONF_LATITUDE) == 0)
        {
            config_manager::instance()->set_latitude(iter->value_fl);
        }
        //else if(iter->key.compare(CONF_FIELD_TEST) == 0)
        //{
        //    config_manager::instance()->set_field_test((u32)iter->value_int);
        //}
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

int config_handler::field_test(void)
{
#if 0
    string version;
    config_manager::instance()->get_sw_version(version);
    if(version.compare("0.0.1") == 0)
    {
        u32 value = config_manager::instance()->get_field_test();
        if(value == 0)
        {
        }
    }
#endif
    return RET_OK;
}

int config_handler::nap_object_list(list<json_param> &obj_list)
{
    // nap-client.conf
    obj_list.push_back(json_param(CONF_GNSS_ENABLE, (int)config_manager::instance()->get_gnss_enable()));
    obj_list.push_back(json_param(CONF_GNSS_CONFIG, (int)config_manager::instance()->get_gnss_config()));
    obj_list.push_back(json_param(CONF_GNSS_INTERVAL, (int)config_manager::instance()->get_gnss_interval()));
    obj_list.push_back(json_param(CONF_GNSS_FIX_TIMEOUT, (int)config_manager::instance()->get_gnss_fix_timeout()));
    obj_list.push_back(json_param(CONF_GNSS_FIX_INTERVAL, (int)config_manager::instance()->get_gnss_fix_interval()));
    obj_list.push_back(json_param(CONF_GNSS_RETRY_CNT, (int)config_manager::instance()->get_gnss_retry_cnt()));

    return RET_OK;
}

int config_handler::nap_read_config(void)
{
    int ret_val = RET_ERROR;
    FILE* fp = NULL;
    string src_data = "";
    char buf[256] = {0,};
    list<json_param> obj_list;
    list<json_param>::iterator iter;

    if(access(CONFIG_NAP, F_OK) == 0)
    {
        fp = fopen(CONFIG_NAP, "rt");
        log_i("%s %s\n", __func__, CONFIG_NAP);
    }
    else if(access(CONFIG_NAP_BAK, F_OK) == 0)
    {
        fp = fopen(CONFIG_NAP_BAK, "rt");
        log_i("%s %s\n", __func__, CONFIG_NAP_BAK);
    }

    if(fp != NULL)
    {
        if(flock(fileno(fp), LOCK_EX) < 0)
        {
            log_i("%s file is locked \n", __func__);
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
        log_w("%s %s open fail !!!\n", __func__, CONFIG_NAP);
        goto free_all;
    }

    nap_object_list(obj_list);
    _json_mgr.config_parse(src_data, obj_list);

    for(iter = obj_list.begin(); iter != obj_list.end(); ++iter)
    {
        if(iter->key.compare(CONF_GNSS_ENABLE) == 0)
        {
            config_manager::instance()->set_gnss_enable((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_GNSS_CONFIG) == 0)
        {
            config_manager::instance()->set_gnss_config((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_GNSS_INTERVAL) == 0)
        {
            config_manager::instance()->set_gnss_interval((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_GNSS_FIX_TIMEOUT) == 0)
        {
            config_manager::instance()->set_gnss_fix_timeout((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_GNSS_FIX_INTERVAL) == 0)
        {
            config_manager::instance()->set_gnss_fix_interval((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_GNSS_RETRY_CNT) == 0)
        {
            config_manager::instance()->set_gnss_retry_cnt((u32)iter->value_int);
        }
    }

    ret_val = RET_OK;

free_all:
    if(fp != NULL) fclose(fp);

    log_i("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

