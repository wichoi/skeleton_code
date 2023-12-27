#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "config-manager.h"

config_manager* config_manager::_singleton_instance = NULL;

config_manager::config_manager()
{
    log_d("%s \n", __func__);
    set_default();
}

config_manager::~config_manager()
{
    log_d("%s\n", __func__);
    delete _singleton_instance;
    _singleton_instance = NULL;
}

config_manager* config_manager::instance()
{
    if(_singleton_instance == NULL)
    {
        _singleton_instance = new config_manager();
    }
    return _singleton_instance;
}

int config_manager::set_default(void)
{
    log_d("%s\n", __func__);

    // cloud config
    _cloud_token = "";
    _cloud_private_key = "";
    _cloud_retry_interval = 1000 * 60 * 60 * 2; // 2hour
    _cloud_report_interval = 1000 * 60 * 30; // 30min
    _cloud_heartbeat_interval = 0;
    _cloud_reboot_reason = "Normal";
    _cloud_reboot_time = "";
    _cloud_recovery_time = "";
    _cloud_notify_net_err = 1;
    _cloud_notify_ip_change = 2;
    _cloud_notify_lan_change = 0;
    _cloud_loglevel = "-I";
    _cloud_sysinfo_time = 1000 * 60 * 15; // 15min

    // cloud fwup
    _cloud_fw_product = "";
    _cloud_fw_version = "";
    _cloud_fw_url = "";
    _cloud_fw_csurl = "";
    _cloud_fw_start_tm = "";
    _cloud_fw_span = 0;
    _cloud_fw_status = "";
    _cloud_fw_reason = "";
    _cloud_fw_code = 0;
    _cloud_fw_desc = "";

    // modem information
    _modem_imei = "";
    _modem_version = "";
    _modem_ver_ex = "";
    _modem_number = "";
    _modem_pci = 0;
    _modem_cell_id = "";
    _modem_frequency = 0;
    _modem_band = 0;
    _modem_band_change = "";
    _modem_apn = "";
    _modem_ip = "";
    _modem_rssi = 0;
    _modem_rsrp = 0;
    _modem_rsrq = 0;
    _modem_sinr = 99;

    // router information
    _dev_serial = "";
    _dev_version = "";
    _dev_ver_ex = "";
    _dev_tx = 0;
    _dev_rx = 0;
    _prev_tx = 0;
    _prev_rx = 0;
    _uptime = "";
    _reset_cnt = 0;
    _ip_change_cnt = 0;
    _net_err_cnt = 0;

    // debug info
    _event_queue_cnt = 0;
    _timer_queue_cnt = 0;
    _cloud_queue_cnt = 0;

    return RET_OK;
}

// =============================================================
// config information
int config_manager::get_fw_name(string &fw_name)
{
    fw_name = _fw_name;
    log_d("%s %s\n", __func__, fw_name.c_str());
    return RET_OK;
}

u32 config_manager::get_config_version(string &version)
{
    version = _config_version;
    log_d("%s %s\n", __func__, version.c_str());
    return RET_OK;
}

u32 config_manager::get_app_version(string &version)
{
    version = _app_version;
    log_d("%s %s\n", __func__, version.c_str());
    return RET_OK;
}

// =============================================================
// fwup url
string& config_manager::get_fwup_url(void)
{
    log_d("%s [%s]\n", __func__, _fwup_url.c_str());
    return _fwup_url;
}

int config_manager::set_fwup_url(string &value)
{
    _fwup_url = value;
    log_d("%s [%s]\n", __func__, _fwup_url.c_str());
    return RET_OK;
}

// =============================================================
// cloud config
string& config_manager::get_cloud_url(void)
{
    log_d("%s [%s]\n", __func__, _cloud_url.c_str());
    return _cloud_url;
}

int config_manager::set_cloud_url(string &value)
{
    _cloud_url = value;
    log_d("%s [%s]\n", __func__, _cloud_url.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_token(void)
{
    log_d("%s [%s]\n", __func__, _cloud_token.c_str());
    return _cloud_token;
}

int config_manager::set_cloud_token(string &value)
{
    _cloud_token = value;
    log_d("%s [%s]\n", __func__, _cloud_token.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_private_key(void)
{
    log_d("%s [%s]\n", __func__, _cloud_private_key.c_str());
    return _cloud_private_key;
}

int config_manager::set_cloud_private_key(string &value)
{
    _cloud_private_key = value;
    log_d("%s [%s]\n", __func__, _cloud_private_key.c_str());
    return RET_OK;
}

u32 config_manager::get_cloud_retry_interval(void)
{
    log_d("%s [%d]\n", __func__, _cloud_retry_interval);
    if(_cloud_retry_interval < 1000 * 60) return 1000 * 60; // 60sec
    return _cloud_retry_interval;
}

int config_manager::set_cloud_retry_interval(u32 value)
{
    _cloud_retry_interval = value;
    log_d("%s [%d]\n", __func__, _cloud_retry_interval);
    return RET_OK;
}

u32 config_manager::get_cloud_report_interval(void)
{
    log_d("%s [%d]\n", __func__, _cloud_report_interval);
    if(_cloud_report_interval < 1000 * 60) return 1000 * 60; // 60sec
    return _cloud_report_interval;
}

int config_manager::set_cloud_report_interval(u32 value)
{
    _cloud_report_interval = value;
    log_d("%s [%d]\n", __func__, _cloud_report_interval);
    return RET_OK;
}

u32 config_manager::get_cloud_heartbeat_interval(void)
{
    log_d("%s [%d]\n", __func__, _cloud_heartbeat_interval);
    return _cloud_heartbeat_interval;
}

int config_manager::set_cloud_heartbeat_interval(u32 value)
{
    _cloud_heartbeat_interval = value;
    log_d("%s [%d]\n", __func__, _cloud_heartbeat_interval);
    return RET_OK;
}

string& config_manager::get_cloud_reboot_reason(void)
{
    log_d("%s [%s]\n", __func__, _cloud_reboot_reason.c_str());
    return _cloud_reboot_reason;
}

int config_manager::set_cloud_reboot_reason(string &value)
{
    _cloud_reboot_reason = value;
    log_d("%s [%s]\n", __func__, _cloud_reboot_reason.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_reboot_time(void)
{
    log_d("%s [%s]\n", __func__, _cloud_reboot_time.c_str());
    return _cloud_reboot_time;
}

int config_manager::set_cloud_reboot_time(string &value)
{
    _cloud_reboot_time = value;
    log_d("%s [%s]\n", __func__, _cloud_reboot_time.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_recovery_time(void)
{
    log_d("%s [%s]\n", __func__, _cloud_recovery_time.c_str());
    return _cloud_recovery_time;
}

int config_manager::set_cloud_recovery_time(string &value)
{
    _cloud_recovery_time = value;
    log_d("%s [%s]\n", __func__, _cloud_recovery_time.c_str());
    return RET_OK;
}

u32 config_manager::get_cloud_notify_net_err(void)
{
    log_d("%s [%d]\n", __func__, _cloud_notify_net_err);
    return _cloud_notify_net_err;
}

int config_manager::set_cloud_notify_net_err(u32 value)
{
    _cloud_notify_net_err = value;
    log_d("%s [%d]\n", __func__, _cloud_notify_net_err);
    return RET_OK;
}

u32 config_manager::get_cloud_notify_ip_change(void)
{
    log_d("%s [%d]\n", __func__, _cloud_notify_ip_change);
    return _cloud_notify_ip_change;
}

int config_manager::set_cloud_notify_ip_change(u32 value)
{
    _cloud_notify_ip_change = value;
    log_d("%s [%d]\n", __func__, _cloud_notify_ip_change);
    return RET_OK;
}

u32 config_manager::get_cloud_notify_lan_change(void)
{
    log_d("%s [%d]\n", __func__, _cloud_notify_lan_change);
    return _cloud_notify_lan_change;
}

int config_manager::set_cloud_notify_lan_change(u32 value)
{
    _cloud_notify_lan_change = value;
    log_d("%s [%d]\n", __func__, _cloud_notify_lan_change);
    return RET_OK;
}

string& config_manager::get_cloud_loglevel(void)
{
    log_d("%s [%s]\n", __func__, _cloud_loglevel.c_str());
    return _cloud_loglevel;
}

int config_manager::set_cloud_loglevel(string &value)
{
    _cloud_loglevel = value;
    log_d("%s [%s]\n", __func__, _cloud_loglevel.c_str());
    return RET_OK;
}

u32 config_manager::get_cloud_sysinfo_time(void)
{
    log_d("%s [%d]\n", __func__, _cloud_sysinfo_time);
    return _cloud_sysinfo_time;
}

int config_manager::set_cloud_sysinfo_time(u32 value)
{
    _cloud_sysinfo_time = value;
    log_d("%s [%d]\n", __func__, _cloud_sysinfo_time);
    return RET_OK;
}

// =============================================================
// cloud fwup
string& config_manager::get_cloud_fw_device(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_product.c_str());
    return _cloud_fw_product;
}

int config_manager::set_cloud_fw_device(string &value)
{
    _cloud_fw_product = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_product.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_fw_version(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_version.c_str());
    return _cloud_fw_version;
}

int config_manager::set_cloud_fw_version(string &value)
{
    _cloud_fw_version = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_version.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_fw_url(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_url.c_str());
    return _cloud_fw_url;
}

int config_manager::set_cloud_fw_url(string &value)
{
    _cloud_fw_url = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_url.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_fw_csurl(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_csurl.c_str());
    return _cloud_fw_csurl;
}

int config_manager::set_cloud_fw_csurl(string &value)
{
    _cloud_fw_csurl = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_csurl.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_fw_start_tm(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_start_tm.c_str());
    return _cloud_fw_start_tm;
}

int config_manager::set_cloud_fw_start_tm(string &value)
{
    _cloud_fw_start_tm = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_start_tm.c_str());
    return RET_OK;
}

u32 config_manager::get_cloud_fw_span(void)
{
    log_d("%s [%d]\n", __func__, _cloud_fw_span);
    return _cloud_fw_span;
}

int config_manager::set_cloud_fw_span(u32 value)
{
    _cloud_fw_span = value;
    log_d("%s [%d]\n", __func__, _cloud_fw_span);
    return RET_OK;
}

string& config_manager::get_cloud_fw_status(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_status.c_str());
    return _cloud_fw_status;
}

int config_manager::set_cloud_fw_status(string &value)
{
    _cloud_fw_status = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_status.c_str());
    return RET_OK;
}

string& config_manager::get_cloud_fw_reason(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_reason.c_str());
    return _cloud_fw_reason;
}

int config_manager::set_cloud_fw_reason(string &value)
{
    _cloud_fw_reason = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_reason.c_str());
    return RET_OK;
}

int config_manager::get_cloud_fw_code(void)
{
    log_d("%s [%d]\n", __func__, _cloud_fw_code);
    return _cloud_fw_code;
}

int config_manager::set_cloud_fw_code(int value)
{
    _cloud_fw_code = value;
    log_d("%s [%d]\n", __func__, _cloud_fw_code);
    return RET_OK;
}

string& config_manager::get_cloud_fw_description(void)
{
    log_d("%s [%s]\n", __func__, _cloud_fw_desc.c_str());
    return _cloud_fw_desc;
}

int config_manager::set_cloud_fw_description(string &value)
{
    _cloud_fw_desc = value;
    log_d("%s [%s]\n", __func__, _cloud_fw_desc.c_str());
    return RET_OK;
}

// =============================================================
// modem information
string& config_manager::get_modem_imei(void)
{
    log_d("%s [%s]\n", __func__, _modem_imei.c_str());
    return _modem_imei;
}

int config_manager::set_modem_imei(string &value)
{
    _modem_imei = value;
    log_d("%s [%s]\n", __func__, _modem_imei.c_str());
    return RET_OK;
}

string& config_manager::get_modem_version(void)
{
    log_d("%s [%s]\n", __func__, _modem_version.c_str());
    return _modem_version;
}

int config_manager::set_modem_version(string &value)
{
    _modem_version = value;
    log_d("%s [%s]\n", __func__, _modem_version.c_str());
    return RET_OK;
}

string& config_manager::get_modem_ver_ex(void)
{
    log_d("%s [%s]\n", __func__, _modem_ver_ex.c_str());
    return _modem_ver_ex;
}

int config_manager::set_modem_ver_ex(string &value)
{
    _modem_ver_ex = value;
    log_d("%s [%s]\n", __func__, _modem_ver_ex.c_str());
    return RET_OK;
}

string& config_manager::get_modem_number(void)
{
    log_d("%s [%s]\n", __func__, _modem_number.c_str());
    return _modem_number;
}

int config_manager::set_modem_number(string &value)
{
    _modem_number = value;
    log_d("%s [%s]\n", __func__, _modem_number.c_str());
    return RET_OK;
}

int config_manager::get_modem_pci(void)
{
    log_d("%s [%d]\n", __func__, _modem_pci);
    return _modem_pci;
}

int config_manager::set_modem_pci(int value)
{
    _modem_pci = value;
    log_d("%s [%d]\n", __func__, _modem_pci);
    return RET_OK;
}

string& config_manager::get_modem_cell_id(void)
{
    log_d("%s [%s]\n", __func__, _modem_cell_id.c_str());
    return _modem_cell_id;
}

int config_manager::set_modem_cell_id(string &value)
{
    _modem_cell_id = value;
    log_d("%s [%s]\n", __func__, _modem_cell_id.c_str());
    return RET_OK;
}

int config_manager::get_modem_frequency(void)
{
    log_d("%s [%d]\n", __func__, _modem_frequency);
    return _modem_frequency;
}

int config_manager::set_modem_frequency(int value)
{
    _modem_frequency = value;
    log_d("%s [%d]\n", __func__, _modem_frequency);
    return RET_OK;
}

int config_manager::get_modem_band(void)
{
    log_d("%s [%d]\n", __func__, _modem_band);
    return _modem_band;
}

int config_manager::set_modem_band(int value)
{
    _modem_band = value;
    log_d("%s [%d]\n", __func__, _modem_band);
    return RET_OK;
}
string& config_manager::get_modem_band_change(void)
{
    log_d("%s [%s]\n", __func__, _modem_band_change.c_str());
    return _modem_band_change;
}

int config_manager::set_modem_band_change(string &value)
{
    _modem_band_change = value;
    log_d("%s [%s]\n", __func__, _modem_band_change.c_str());
    return RET_OK;
}

string& config_manager::get_modem_apn(void)
{
    log_d("%s [%s]\n", __func__, _modem_apn.c_str());
    return _modem_apn;
}

int config_manager::set_modem_apn(string &value)
{
    _modem_apn = value;
    log_d("%s [%s]\n", __func__, _modem_apn.c_str());
    return RET_OK;
}

string& config_manager::get_modem_ip_addr(void)
{
    log_d("%s [%s]\n", __func__, _modem_ip.c_str());
    return _modem_ip;
}

int config_manager::set_modem_ip_addr(string &value)
{
    _modem_ip = value;
    log_d("%s [%s]\n", __func__, _modem_ip.c_str());
    return RET_OK;
}

int config_manager::get_modem_rssi(void)
{
    log_d("%s [%d]\n", __func__, _modem_rssi);
    return _modem_rssi;
}

int config_manager::set_modem_rssi(int value)
{
    if(value > 0) _modem_rssi = -value;
    else          _modem_rssi =  value;
    log_d("%s [%d]\n", __func__, _modem_rssi);
    return RET_OK;
}

int config_manager::get_modem_rsrp(void)
{
    log_d("%s [%d]\n", __func__, _modem_rsrp);
    return _modem_rsrp;
}

int config_manager::set_modem_rsrp(int value)
{
    _modem_rsrp = value;
    log_d("%s [%d]\n", __func__, _modem_rsrp);
    return RET_OK;
}

int config_manager::get_modem_rsrq(void)
{
    log_d("%s [%d]\n", __func__, _modem_rsrq);
    return _modem_rsrq;
}

int config_manager::set_modem_rsrq(int value)
{
    _modem_rsrq = value;
    log_d("%s [%d]\n", __func__, _modem_rsrq);
    return RET_OK;
}

int config_manager::get_modem_sinr(void)
{
    log_d("%s [%d]\n", __func__, _modem_sinr);
    return _modem_sinr;
}

int config_manager::set_modem_sinr(int value)
{
    _modem_sinr = value;
    log_d("%s [%d]\n", __func__, _modem_sinr);
    return RET_OK;
}

// =============================================================
// router information
string& config_manager::get_router_model(void)
{
    log_d("%s [%s]\n", __func__, _dev_model.c_str());
    return _dev_model;
}

int config_manager::set_router_model(string &value)
{
    _dev_model = value;
    log_d("%s [%s]\n", __func__, _dev_model.c_str());
    return RET_OK;
}

string& config_manager::get_router_serial(void)
{
    log_d("%s [%s]\n", __func__, _dev_serial.c_str());
    return _dev_serial;
}

int config_manager::set_router_serial(string &value)
{
    _dev_serial = value;
    log_d("%s [%s]\n", __func__, _dev_serial.c_str());
    return RET_OK;
}

string& config_manager::get_router_version(void)
{
    log_d("%s [%s]\n", __func__, _dev_version.c_str());
    return _dev_version;
}

int config_manager::set_router_version(string &value)
{
    _dev_version = value;
    log_d("%s [%s]\n", __func__, _dev_version.c_str());
    return RET_OK;
}

string& config_manager::get_router_ver_ex(void)
{
    log_d("%s [%s]\n", __func__, _dev_ver_ex.c_str());
    return _dev_ver_ex;
}

int config_manager::set_router_ver_ex(string &value)
{
    _dev_ver_ex = value;
    log_d("%s [%s]\n", __func__, _dev_ver_ex.c_str());
    return RET_OK;
}

u32 config_manager::get_router_tx(void)
{
    log_d("%s [%d]\n", __func__, _dev_tx);
    return _dev_tx;
}

int config_manager::set_router_tx(u32 value)
{
    _dev_tx = value;
    log_d("%s [%d]\n", __func__, _dev_tx);
    return RET_OK;
}

u32 config_manager::get_router_rx(void)
{
    log_d("%s [%d]\n", __func__, _dev_rx);
    return _dev_rx;
}

int config_manager::set_router_rx(u32 value)
{
    _dev_rx = value;
    log_d("%s [%d]\n", __func__, _dev_rx);
    return RET_OK;
}

u32 config_manager::get_router_prev_tx(void)
{
    log_d("%s [%d]\n", __func__, _prev_tx);
    return _prev_tx;
}

int config_manager::set_router_prev_tx(u32 value)
{
    _prev_tx = value;
    log_d("%s [%d]\n", __func__, _prev_tx);
    return RET_OK;
}

u32 config_manager::get_router_prev_rx(void)
{
    log_d("%s [%d]\n", __func__, _prev_rx);
    return _prev_rx;
}

int config_manager::set_router_prev_rx(u32 value)
{
    _prev_rx = value;
    log_d("%s [%d]\n", __func__, _prev_rx);
    return RET_OK;
}

string& config_manager::get_router_uptime(void)
{
    log_d("%s [%s]\n", __func__, _uptime.c_str());
    return _uptime;
}

int config_manager::set_router_uptime(string &value)
{
    _uptime = value;
    log_d("%s [%s]\n", __func__, _uptime.c_str());
    return RET_OK;
}

u32 config_manager::get_modem_reset_cnt(void)
{
    log_d("%s [%d]\n", __func__, _reset_cnt);
    return _reset_cnt;
}

int config_manager::set_modem_reset_cnt(u32 value)
{
    _reset_cnt = value;
    log_d("%s [%d]\n", __func__, _reset_cnt);
    return RET_OK;
}

u32 config_manager::get_ip_change_cnt(void)
{
    log_d("%s [%d]\n", __func__, _ip_change_cnt);
    return _ip_change_cnt;
}

int config_manager::set_ip_change_cnt(u32 value)
{
    _ip_change_cnt = value;
    log_d("%s [%d]\n", __func__, _ip_change_cnt);
    return RET_OK;
}

u32 config_manager::get_network_err_cnt(void)
{
    log_d("%s [%d]\n", __func__, _net_err_cnt);
    return _net_err_cnt;
}

int config_manager::set_network_err_cnt(u32 value)
{
    _net_err_cnt = value;
    log_d("%s [%d]\n", __func__, _net_err_cnt);
    return RET_OK;
}

u32 config_manager::get_event_queue_cnt(void)
{
    log_d("%s [%d]\n", __func__, _event_queue_cnt);
    return _event_queue_cnt;
}

int config_manager::set_event_queue_cnt(int value)
{
    _event_queue_cnt = value;
    log_d("%s [%d]\n", __func__, _event_queue_cnt);
    return RET_OK;
}

u32 config_manager::get_timer_queue_cnt(void)
{
    log_d("%s [%d]\n", __func__, _timer_queue_cnt);
    return _timer_queue_cnt;
}

int config_manager::set_timer_queue_cnt(int value)
{
    _timer_queue_cnt = value;
    log_d("%s [%d]\n", __func__, _timer_queue_cnt);
    return RET_OK;
}

u32 config_manager::get_cloud_queue_cnt(void)
{
    log_d("%s [%d]\n", __func__, _cloud_queue_cnt);
    return _cloud_queue_cnt;
}

int config_manager::set_cloud_queue_cnt(int value)
{
    _cloud_queue_cnt = value;
    log_d("%s [%d]\n", __func__, _cloud_queue_cnt);
    return RET_OK;
}

