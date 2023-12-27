#ifndef __CONFIG_MANAGER_H__
#define __CONFIG_MANAGER_H__

#include <string>

#include "common.h"

class config_manager
{
private:
    config_manager();
    ~config_manager();

public:
    static config_manager* instance(void);
    int set_default(void);

// =============================================================
// config information
    int get_fw_name(string &fw_name);
    u32 get_config_version(string &version);
    u32 get_app_version(string &version);

// =============================================================
// fwup url
    string& get_fwup_url(void);
    int set_fwup_url(string &value);

// =============================================================
// cloud config
    string& get_cloud_url(void);
    int set_cloud_url(string &value);

    string& get_cloud_token(void);
    int set_cloud_token(string &value);

    string& get_cloud_private_key(void);
    int set_cloud_private_key(string &value);

    u32 get_cloud_retry_interval(void);
    int set_cloud_retry_interval(u32 value);

    u32 get_cloud_report_interval(void);
    int set_cloud_report_interval(u32 value);

    u32 get_cloud_heartbeat_interval(void);
    int set_cloud_heartbeat_interval(u32 value);

    string& get_cloud_reboot_reason(void);
    int set_cloud_reboot_reason(string &value);

    string& get_cloud_reboot_time(void);
    int set_cloud_reboot_time(string &value);

    string& get_cloud_recovery_time(void);
    int set_cloud_recovery_time(string &value);

    u32 get_cloud_notify_net_err(void);
    int set_cloud_notify_net_err(u32 value);

    u32 get_cloud_notify_ip_change(void);
    int set_cloud_notify_ip_change(u32 value);

    u32 get_cloud_notify_lan_change(void);
    int set_cloud_notify_lan_change(u32 value);

    string& get_cloud_loglevel(void);
    int set_cloud_loglevel(string &value);

    u32 get_cloud_sysinfo_time(void);
    int set_cloud_sysinfo_time(u32 value);

// =============================================================
// cloud fwup

    string& get_cloud_fw_device(void);
    int set_cloud_fw_device(string &value);

    string& get_cloud_fw_version(void);
    int set_cloud_fw_version(string &value);

    string& get_cloud_fw_url(void);
    int set_cloud_fw_url(string &value);

    string& get_cloud_fw_csurl(void);
    int set_cloud_fw_csurl(string &value);

    string& get_cloud_fw_start_tm(void);
    int set_cloud_fw_start_tm(string &value);

    u32 get_cloud_fw_span(void);
    int set_cloud_fw_span(u32 value);

    string& get_cloud_fw_status(void);
    int set_cloud_fw_status(string &value);

    string& get_cloud_fw_reason(void);
    int set_cloud_fw_reason(string &value);

    int get_cloud_fw_code(void);
    int set_cloud_fw_code(int value);

    string& get_cloud_fw_description(void);
    int set_cloud_fw_description(string &value);

// =============================================================
// modem information
    string& get_modem_imei(void);
    int set_modem_imei(string &value);

    string& get_modem_version(void);
    int set_modem_version(string &value);

    string& get_modem_ver_ex(void);
    int set_modem_ver_ex(string &value);

    string& get_modem_number(void);
    int set_modem_number(string &value);

    int get_modem_pci(void);
    int set_modem_pci(int value);

    string& get_modem_cell_id(void);
    int set_modem_cell_id(string &value);

    int get_modem_frequency(void);
    int set_modem_frequency(int value);

    int get_modem_band(void);
    int set_modem_band(int value);

    string& get_modem_band_change(void);
    int set_modem_band_change(string &value);

    string& get_modem_apn(void);
    int set_modem_apn(string &value);

    string& get_modem_ip_addr(void);
    int set_modem_ip_addr(string &value);

    int get_modem_rssi(void);
    int set_modem_rssi(int value);

    int get_modem_rsrp(void);
    int set_modem_rsrp(int value);

    int get_modem_rsrq(void);
    int set_modem_rsrq(int value);

    int get_modem_sinr(void);
    int set_modem_sinr(int value);

// =============================================================
// router information
    string& get_router_model(void);
    int set_router_model(string &value);

    string& get_router_serial(void);
    int set_router_serial(string &value);

    string& get_router_version(void);
    int set_router_version(string &value);

    string& get_router_ver_ex(void);
    int set_router_ver_ex(string &value);

    u32 get_router_tx(void);
    int set_router_tx(u32 value);

    u32 get_router_rx(void);
    int set_router_rx(u32 value);

    u32 get_router_prev_tx(void);
    int set_router_prev_tx(u32 value);

    u32 get_router_prev_rx(void);
    int set_router_prev_rx(u32 value);

    string& get_router_uptime(void);
    int set_router_uptime(string &value);

    u32 get_modem_reset_cnt(void);
    int set_modem_reset_cnt(u32 value);

    u32 get_ip_change_cnt(void);
    int set_ip_change_cnt(u32 value);

    u32 get_network_err_cnt(void);
    int set_network_err_cnt(u32 value);

    // debug info
    u32 get_event_queue_cnt(void);
    int set_event_queue_cnt(int value);

    u32 get_timer_queue_cnt(void);
    int set_timer_queue_cnt(int value);

    u32 get_cloud_queue_cnt(void);
    int set_cloud_queue_cnt(int value);

private:
    static config_manager *_singleton_instance;

    // config information
    string _fw_name;
    string _config_version;
    string _app_version;

    // fwup url
    string _fwup_url;

    // cloud config
    string _cloud_url;
    string _cloud_token;
    string _cloud_private_key;
    u32 _cloud_retry_interval;
    u32 _cloud_report_interval;
    u32 _cloud_heartbeat_interval;
    string _cloud_reboot_reason;
    string _cloud_reboot_time;
    string _cloud_recovery_time;
    u32 _cloud_notify_net_err;
    u32 _cloud_notify_ip_change;
    u32 _cloud_notify_lan_change;
    string _cloud_loglevel;
    u32 _cloud_sysinfo_time;

    // cloud fwup
    string _cloud_fw_product;
    string _cloud_fw_version;
    string _cloud_fw_url;
    string _cloud_fw_csurl;
    string _cloud_fw_start_tm;
    u32 _cloud_fw_span;

    // cloud fwup result
    string _cloud_fw_status;
    string _cloud_fw_reason;
    int _cloud_fw_code;
    string _cloud_fw_desc;

    // modem information
    string _modem_imei;
    string _modem_version;
    string _modem_ver_ex;
    string _modem_number;
    int _modem_pci;
    string _modem_cell_id;
    int _modem_frequency;
    int _modem_band;
    string _modem_band_change;
    string _modem_apn;
    string _modem_ip;
    int _modem_rssi;
    int _modem_rsrp;
    int _modem_rsrq;
    int _modem_sinr;

    // router information
    string _dev_model;
    string _dev_serial;
    string _dev_version;
    string _dev_ver_ex;
    u32 _dev_tx;
    u32 _dev_rx;
    u32 _prev_tx;
    u32 _prev_rx;
    string _uptime;
    u32 _reset_cnt;
    u32 _ip_change_cnt;
    u32 _net_err_cnt;

    // debug info
    int _event_queue_cnt;
    int _timer_queue_cnt;
    int _cloud_queue_cnt;
};

#endif
