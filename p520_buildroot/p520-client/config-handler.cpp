#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#include "log.h"
#include "utils.h"
#include "config-handler.h"
#include "config-manager.h"

#define CONFIG_FILE                 "/tmp/p520-client.conf"
#define CONFIG_FILE_BAK             "/tmp/p520-client.conf.bak"

// config information
#define CONF_WRITE_TIME             "conf_time"
#define CONF_FW_NAME                "conf_name"
#define CONF_VERSION                "conf_version"
#define CONF_APP_VERSION            "app_version"

// fwup url
#define CONF_URL_FW                 "fwup_url"

// cloud config
#define CONF_CLOUD_URL              "cloud_url"
#define CONF_CLOUD_TOKEN            "cloud_token"
#define CONF_CLOUD_KEY              "cloud_key"
#define CONF_CLOUD_RETRY_MSEC       "cloud_retry_msec"
#define CONF_CLOUD_RPT_MSEC         "cloud_report_msec"
#define CONF_CLOUD_HB_MSEC          "cloud_heartbeat_msec"
#define CONF_CLOUD_REBOOT_REASON    "cloud_reboot_reason"
#define CONF_CLOUD_REBOOT_TIME      "cloud_reboot_time"
#define CONF_CLOUD_RECOVERY_TIME    "cloud_recovery_time"
#define CONF_CLOUD_NOTI_NET_ERR     "cloud_notify_net_err"
#define CONF_CLOUD_NOTI_IP_CHANGE   "cloud_notify_ip_change"
#define CONF_CLOUD_NOTI_LAN_CHANGE  "cloud_notify_lan_change"
#define CONF_CLOUD_LOG              "cloud_loglevel"
#define CONF_CLOUD_SYS_TIME         "cloud_sysinfo_time"

// cloud fwup
#define CONF_CLOUD_FW_DEV           "cloud_fw_dev"
#define CONF_CLOUD_FW_VER           "cloud_fw_ver"
#define CONF_CLOUD_FW_URL           "cloud_fw_url"
#define CONF_CLOUD_FW_CS            "cloud_fw_csurl"
#define CONF_CLOUD_FW_TM            "cloud_fw_tm"
#define CONF_CLOUD_FW_SPAN          "cloud_fw_span"
#define CONF_CLOUD_FW_ST            "cloud_fw_st"
#define CONF_CLOUD_FW_REASON        "cloud_fw_reason"
#define CONF_CLOUD_FW_CODE          "cloud_fw_code"
#define CONF_CLOUD_FW_DESC          "cloud_fw_desc"

// modem information
#define CONF_MODEM_IMEI             "modem_imei"
#define CONF_MODEM_VERSION          "modem_version"
#define CONF_MODEM_VER_EX           "modem_ver_ex"
#define CONF_MODEM_NUMBER           "modem_number"
#define CONF_MODEM_PCI              "modem_pci"
#define CONF_MODEM_CELL_ID          "modem_cell_id"
#define CONF_MODEM_FREQUENCY        "modem_frequecny"
#define CONF_MODEM_BAND             "modem_band"
#define CONF_MODEM_BAND_CHANGE      "modem_band_change"
#define CONF_MODEM_APN              "modem_apn"
#define CONF_MODEM_IP_ADDR          "modem_ip_addr"
#define CONF_MODEM_RSSI             "modem_rssi"
#define CONF_MODEM_RSRP             "modem_rsrp"
#define CONF_MODEM_RSRQ             "modem_rsrq"
#define CONF_MODEM_SINR             "modem_sinr"

// router information
#define CONF_ROUTER_MODEL           "router_model"
#define CONF_ROUTER_SERIAL          "router_serial"
#define CONF_ROUTER_VERSION         "router_version"
#define CONF_ROUTER_VER_EX          "router_ver_ex"
#define CONF_ROUTER_TX              "router_tx"
#define CONF_ROUTER_RX              "router_rx"
#define CONF_ROUTER_PREV_TX         "router_prev_tx"
#define CONF_ROUTER_PREV_RX         "router_prev_rx"
#define CONF_ROUTER_UPTIME          "router_uptime"
#define CONF_MDM_RESET_CNT          "mdm_reset_cnt"
#define CONF_IP_CHANGE_CNT          "ip_change_cnt"
#define CONF_NETWORK_ERR_CNT        "net_err_cnt"

// debug information
#define CONF_EVENT_QUEUE_CNT        "event_queue_cnt"
#define CONF_TIMER_QUEUE_CNT        "timer_queue_cnt"
#define CONF_CLOUD_QUEUE_CNT        "cloud_queue_cnt"

// nvram nvitem
#define NVITEM_NET_ERR_CNT          "AproNetErrCnt"
#define NVITEM_REBOOT_REASON        "AproRebootReason"
#define NVITEM_REBOOT_REASON_EX     "AproRebootReasonEx"
#define NVITEM_REBOOT_TIME          "AproRebootTime"
#define NVITEM_CLOUD_TOKEN          "AproCloudToken"
#define NVITEM_CLOUD_KEY            "AproCloudKey"
#define NVITEM_CLOUD_URL            "AproCloudUrl"
#define NVITEM_CLOUD_LOG            "AproCloudLog"
#define NVITEM_CLOUD_FW_VER         "AproCloudFwVer"
#define NVITEM_CLOUD_FW_TYPE        "AproCloudFwType"
#define NVITEM_CLOUD_FW_ST          "AproCloudFwSt"

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
    write_config();
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
    string fw_name;
    config_manager::instance()->get_fw_name(fw_name);
    string sw_ver;
    config_manager::instance()->get_config_version(sw_ver);
    string app_ver;
    config_manager::instance()->get_app_version(app_ver);
    obj_list.push_back(json_param(CONF_WRITE_TIME, current_time));
    obj_list.push_back(json_param(CONF_FW_NAME, fw_name));
    obj_list.push_back(json_param(CONF_VERSION, sw_ver));
    obj_list.push_back(json_param(CONF_APP_VERSION, app_ver));
    obj_list.push_back(json_param(CONF_URL_FW, config_manager::instance()->get_fwup_url()));
    obj_list.push_back(json_param(CONF_CLOUD_URL, config_manager::instance()->get_cloud_url()));
    obj_list.push_back(json_param(CONF_CLOUD_TOKEN, config_manager::instance()->get_cloud_token()));
    //obj_list.push_back(json_param(CONF_CLOUD_KEY, config_manager::instance()->get_cloud_private_key()));
    obj_list.push_back(json_param(CONF_CLOUD_RETRY_MSEC, (int)config_manager::instance()->get_cloud_retry_interval()));
    obj_list.push_back(json_param(CONF_CLOUD_RPT_MSEC, (int)config_manager::instance()->get_cloud_report_interval()));
    obj_list.push_back(json_param(CONF_CLOUD_HB_MSEC, (int)config_manager::instance()->get_cloud_heartbeat_interval()));
    obj_list.push_back(json_param(CONF_CLOUD_REBOOT_REASON, config_manager::instance()->get_cloud_reboot_reason()));
    obj_list.push_back(json_param(CONF_CLOUD_REBOOT_TIME, config_manager::instance()->get_cloud_reboot_time()));
    obj_list.push_back(json_param(CONF_CLOUD_RECOVERY_TIME, config_manager::instance()->get_cloud_recovery_time()));
    obj_list.push_back(json_param(CONF_CLOUD_NOTI_NET_ERR, (int)config_manager::instance()->get_cloud_notify_net_err()));
    obj_list.push_back(json_param(CONF_CLOUD_NOTI_IP_CHANGE, (int)config_manager::instance()->get_cloud_notify_ip_change()));
    obj_list.push_back(json_param(CONF_CLOUD_NOTI_LAN_CHANGE, (int)config_manager::instance()->get_cloud_notify_lan_change()));
    obj_list.push_back(json_param(CONF_CLOUD_LOG, config_manager::instance()->get_cloud_loglevel()));
    obj_list.push_back(json_param(CONF_CLOUD_SYS_TIME, (int)config_manager::instance()->get_cloud_sysinfo_time()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_DEV, config_manager::instance()->get_cloud_fw_device()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_VER, config_manager::instance()->get_cloud_fw_version()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_URL, config_manager::instance()->get_cloud_fw_url()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_CS, config_manager::instance()->get_cloud_fw_csurl()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_TM, config_manager::instance()->get_cloud_fw_start_tm()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_SPAN, (int)config_manager::instance()->get_cloud_fw_span()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_ST, config_manager::instance()->get_cloud_fw_status()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_REASON, config_manager::instance()->get_cloud_fw_reason()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_CODE, (int)config_manager::instance()->get_cloud_fw_code()));
    obj_list.push_back(json_param(CONF_CLOUD_FW_DESC, config_manager::instance()->get_cloud_fw_description()));
    obj_list.push_back(json_param(CONF_MODEM_IMEI, config_manager::instance()->get_modem_imei()));
    obj_list.push_back(json_param(CONF_MODEM_VERSION, config_manager::instance()->get_modem_version()));
    obj_list.push_back(json_param(CONF_MODEM_VER_EX, config_manager::instance()->get_modem_ver_ex()));
    obj_list.push_back(json_param(CONF_MODEM_NUMBER, config_manager::instance()->get_modem_number()));
    obj_list.push_back(json_param(CONF_MODEM_PCI, (int)config_manager::instance()->get_modem_pci()));
    obj_list.push_back(json_param(CONF_MODEM_CELL_ID, config_manager::instance()->get_modem_cell_id()));
    obj_list.push_back(json_param(CONF_MODEM_FREQUENCY, (int)config_manager::instance()->get_modem_frequency()));
    obj_list.push_back(json_param(CONF_MODEM_BAND, (int)config_manager::instance()->get_modem_band()));
    obj_list.push_back(json_param(CONF_MODEM_BAND_CHANGE, config_manager::instance()->get_modem_band_change()));
    obj_list.push_back(json_param(CONF_MODEM_APN, config_manager::instance()->get_modem_apn()));
    obj_list.push_back(json_param(CONF_MODEM_IP_ADDR, config_manager::instance()->get_modem_ip_addr()));
    obj_list.push_back(json_param(CONF_MODEM_RSSI, (int)config_manager::instance()->get_modem_rssi()));
    obj_list.push_back(json_param(CONF_MODEM_RSRP, (int)config_manager::instance()->get_modem_rsrp()));
    obj_list.push_back(json_param(CONF_MODEM_RSRQ, (int)config_manager::instance()->get_modem_rsrq()));
    obj_list.push_back(json_param(CONF_MODEM_SINR, (int)config_manager::instance()->get_modem_sinr()));
    obj_list.push_back(json_param(CONF_ROUTER_SERIAL, config_manager::instance()->get_router_serial()));
    obj_list.push_back(json_param(CONF_ROUTER_MODEL, config_manager::instance()->get_router_model()));
    obj_list.push_back(json_param(CONF_ROUTER_VERSION, config_manager::instance()->get_router_version()));
    obj_list.push_back(json_param(CONF_ROUTER_VER_EX, config_manager::instance()->get_router_ver_ex()));
    obj_list.push_back(json_param(CONF_ROUTER_TX, (int)config_manager::instance()->get_router_tx()));
    obj_list.push_back(json_param(CONF_ROUTER_RX, (int)config_manager::instance()->get_router_rx()));
    obj_list.push_back(json_param(CONF_ROUTER_PREV_TX, (int)config_manager::instance()->get_router_prev_tx()));
    obj_list.push_back(json_param(CONF_ROUTER_PREV_RX, (int)config_manager::instance()->get_router_prev_rx()));
    obj_list.push_back(json_param(CONF_ROUTER_UPTIME, config_manager::instance()->get_router_uptime()));
    obj_list.push_back(json_param(CONF_MDM_RESET_CNT, (int)config_manager::instance()->get_modem_reset_cnt()));
    obj_list.push_back(json_param(CONF_IP_CHANGE_CNT, (int)config_manager::instance()->get_ip_change_cnt()));
    obj_list.push_back(json_param(CONF_NETWORK_ERR_CNT, (int)config_manager::instance()->get_network_err_cnt()));
    obj_list.push_back(json_param(CONF_EVENT_QUEUE_CNT, (int)config_manager::instance()->get_event_queue_cnt()));
    obj_list.push_back(json_param(CONF_TIMER_QUEUE_CNT, (int)config_manager::instance()->get_timer_queue_cnt()));
    obj_list.push_back(json_param(CONF_CLOUD_QUEUE_CNT, (int)config_manager::instance()->get_cloud_queue_cnt()));

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
            fclose(fp);
            goto free_all;
        }

        while(!feof(fp))
        {
            if(fgets(buf, sizeof(buf), fp) != NULL)
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
        if(iter->key.compare(CONF_URL_FW) == 0)
        {
            config_manager::instance()->set_fwup_url(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_URL) == 0)
        {
            config_manager::instance()->set_cloud_url(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_TOKEN) == 0)
        {
            config_manager::instance()->set_cloud_token(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_KEY) == 0)
        {
            config_manager::instance()->set_cloud_private_key(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_RETRY_MSEC) == 0)
        {
            config_manager::instance()->set_cloud_retry_interval((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_RPT_MSEC) == 0)
        {
            config_manager::instance()->set_cloud_report_interval((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_HB_MSEC) == 0)
        {
            config_manager::instance()->set_cloud_heartbeat_interval((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_REBOOT_REASON) == 0)
        {
            config_manager::instance()->set_cloud_reboot_reason(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_REBOOT_TIME) == 0)
        {
            config_manager::instance()->set_cloud_reboot_time(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_RECOVERY_TIME) == 0)
        {
            config_manager::instance()->set_cloud_recovery_time(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_NOTI_NET_ERR) == 0)
        {
            config_manager::instance()->set_cloud_notify_net_err((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_NOTI_IP_CHANGE) == 0)
        {
            config_manager::instance()->set_cloud_notify_ip_change((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_NOTI_LAN_CHANGE) == 0)
        {
            config_manager::instance()->set_cloud_notify_lan_change((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_LOG) == 0)
        {
            config_manager::instance()->set_cloud_loglevel(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_SYS_TIME) == 0)
        {
            config_manager::instance()->set_cloud_sysinfo_time((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_DEV) == 0)
        {
            config_manager::instance()->set_cloud_fw_device(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_VER) == 0)
        {
            config_manager::instance()->set_cloud_fw_version(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_URL) == 0)
        {
            config_manager::instance()->set_cloud_fw_url(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_CS) == 0)
        {
            config_manager::instance()->set_cloud_fw_csurl(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_TM) == 0)
        {
            config_manager::instance()->set_cloud_fw_start_tm(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_SPAN) == 0)
        {
            config_manager::instance()->set_cloud_fw_span(iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_ST) == 0)
        {
            config_manager::instance()->set_cloud_fw_status(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_REASON) == 0)
        {
            config_manager::instance()->set_cloud_fw_reason(iter->value_str);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_CODE) == 0)
        {
            config_manager::instance()->set_cloud_fw_code(iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_FW_DESC) == 0)
        {
            config_manager::instance()->set_cloud_fw_description(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_IMEI) == 0)
        {
            config_manager::instance()->set_modem_imei(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_VERSION) == 0)
        {
            config_manager::instance()->set_modem_version(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_VER_EX) == 0)
        {
            config_manager::instance()->set_modem_ver_ex(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_NUMBER) == 0)
        {
            config_manager::instance()->set_modem_number(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_PCI) == 0)
        {
            config_manager::instance()->set_modem_pci(iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_CELL_ID) == 0)
        {
            config_manager::instance()->set_modem_cell_id(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_FREQUENCY) == 0)
        {
            config_manager::instance()->set_modem_frequency(iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_BAND) == 0)
        {
            config_manager::instance()->set_modem_band(iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_BAND_CHANGE) == 0)
        {
            config_manager::instance()->set_modem_band_change(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_APN) == 0)
        {
            config_manager::instance()->set_modem_apn(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_IP_ADDR) == 0)
        {
            config_manager::instance()->set_modem_ip_addr(iter->value_str);
        }
        else if(iter->key.compare(CONF_MODEM_RSSI) == 0)
        {
            config_manager::instance()->set_modem_rssi(iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_RSRP) == 0)
        {
            config_manager::instance()->set_modem_rsrp(iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_RSRQ) == 0)
        {
            config_manager::instance()->set_modem_rsrq(iter->value_int);
        }
        else if(iter->key.compare(CONF_MODEM_SINR) == 0)
        {
            config_manager::instance()->set_modem_sinr(iter->value_int);
        }
        else if(iter->key.compare(CONF_ROUTER_MODEL) == 0)
        {
            config_manager::instance()->set_router_model(iter->value_str);
        }
        else if(iter->key.compare(CONF_ROUTER_SERIAL) == 0)
        {
            config_manager::instance()->set_router_serial(iter->value_str);
        }
        else if(iter->key.compare(CONF_ROUTER_VERSION) == 0)
        {
            config_manager::instance()->set_router_version(iter->value_str);
        }
        else if(iter->key.compare(CONF_ROUTER_VER_EX) == 0)
        {
            config_manager::instance()->set_router_ver_ex(iter->value_str);
        }
        else if(iter->key.compare(CONF_ROUTER_TX) == 0)
        {
            config_manager::instance()->set_router_tx((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_ROUTER_RX) == 0)
        {
            config_manager::instance()->set_router_rx((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_ROUTER_PREV_TX) == 0)
        {
            config_manager::instance()->set_router_prev_tx((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_ROUTER_PREV_RX) == 0)
        {
            config_manager::instance()->set_router_prev_rx((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_ROUTER_UPTIME) == 0)
        {
            config_manager::instance()->set_router_uptime(iter->value_str);
        }
        else if(iter->key.compare(CONF_MDM_RESET_CNT) == 0)
        {
            config_manager::instance()->set_modem_reset_cnt((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_IP_CHANGE_CNT) == 0)
        {
            config_manager::instance()->set_ip_change_cnt((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_NETWORK_ERR_CNT) == 0)
        {
            config_manager::instance()->set_network_err_cnt((u32)iter->value_int);
        }
        else if(iter->key.compare(CONF_EVENT_QUEUE_CNT) == 0)
        {
            config_manager::instance()->set_event_queue_cnt((int)iter->value_int);
        }
        else if(iter->key.compare(CONF_TIMER_QUEUE_CNT) == 0)
        {
            config_manager::instance()->set_timer_queue_cnt((int)iter->value_int);
        }
        else if(iter->key.compare(CONF_CLOUD_QUEUE_CNT) == 0)
        {
            config_manager::instance()->set_cloud_queue_cnt((int)iter->value_int);
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
    string key = "";
    string value = "";

#if 0
    key = NVITEM_NET_ERR_CNT;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_network_err_cnt((u32)atoi(value.c_str()));
    }
#endif

    key = NVITEM_REBOOT_REASON;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_reboot_reason(value);
    }

    key = NVITEM_REBOOT_REASON_EX;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_reboot_reason(value);

        key = NVITEM_REBOOT_REASON_EX;
        value.clear();
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_REBOOT_TIME;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_reboot_time(value);
    }

    key = NVITEM_CLOUD_TOKEN;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_token(value);
    }

    key = NVITEM_CLOUD_KEY;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_private_key(value);
    }

    key = NVITEM_CLOUD_URL;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_url(value);
    }

    key = NVITEM_CLOUD_LOG;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_loglevel(value);
    }

    key = NVITEM_CLOUD_FW_VER;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_fw_version(value);
    }

    key = NVITEM_CLOUD_FW_TYPE;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_fw_device(value);
    }

    key = NVITEM_CLOUD_FW_ST;
    value.clear();
    utils::nvram_get_2860(key, value);
    if(value.length() > 0)
    {
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
        config_manager::instance()->set_cloud_fw_status(value);
    }

    return RET_OK;
}

int config_handler::write_nvram(void)
{
    string key = "";
    string value = "";
    string nv_val = "";
    //char buf[32] = {0,};

#if 0
    key = NVITEM_NET_ERR_CNT;
    snprintf(buf, sizeof(buf) - 1, "%u", config_manager::instance()->get_network_err_cnt());
    value = buf;
    utils::nvram_set_2860(key, value);
    log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
#endif

    key = NVITEM_REBOOT_REASON;
    value = config_manager::instance()->get_cloud_reboot_reason();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_CLOUD_TOKEN;
    value = config_manager::instance()->get_cloud_token();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());\
    }

    key = NVITEM_CLOUD_KEY;
    value = config_manager::instance()->get_cloud_private_key();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_CLOUD_URL;
    value = config_manager::instance()->get_cloud_url();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_CLOUD_LOG;
    value = config_manager::instance()->get_cloud_loglevel();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_CLOUD_FW_VER;
    value = config_manager::instance()->get_cloud_fw_version();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_CLOUD_FW_TYPE;
    value = config_manager::instance()->get_cloud_fw_device();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    key = NVITEM_CLOUD_FW_ST;
    value = config_manager::instance()->get_cloud_fw_status();
    nv_val.clear();
    utils::nvram_get_2860(key, nv_val);
    if(value.compare(nv_val) != 0)
    {
        utils::nvram_set_2860(key, value);
        log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    }

    return RET_OK;
}

