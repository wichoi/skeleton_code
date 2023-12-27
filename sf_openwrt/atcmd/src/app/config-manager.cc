#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <list>

#include "log.h"
#include "config-manager.h"

// fw name
#define FIRMWARE_NAME       "at-client"

// sw version
#define SW_VER_MAJOR        0
#define SW_VER_MINOR        0
#define SW_VER_BUILD        1

config_manager* config_manager::_singleton_instance = NULL;

config_manager::config_manager()
{
    log_d("%s \n", __func__);
    set_default();
}

config_manager::~config_manager()
{
    log_d("%s\n", __func__);
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

    // at-handle
    _at_dev_path = "/dev/ttyS2";
    _at_buf_sz = 1024;
    _at_timeout = 2000;
    _at_in_progress = 0; // event_c::CMD_NONE

    // modem proc
    _mod_enable = 1;
    _mod_timeout = 3000;
    _mod_state = 0;
    _gnss_state = 0;
    _imei = "";
    _revision = "";
    _version = "";
    _imsi = "";
    _cpin = "";
    _qccid = "";
    _net_operator = "";
    _access_technology = "";
    _operator_num = "";
    _band = "";
    _channel = 0;
    _sys_mode = "";
    _rssi = 0;
    _rsrp = 0;
    _sinr = 0;
    _rsrq = 0;
    _fnn = "";
    _snn = "";
    _spn = "";
    _rplwn = "";
    _pdp_type = "";
    _apn = "";
    _pdp_addr = "";
    _cclk = "";
    _usim = "";
    _number = "";

    // location
    _gnss_enable = 0;
    _gnss_config =  5;
    _gnss_interval = 1000 * 60 * 30; // 30 min
    _gnss_fix_timeout = 1000 * 50; // 50 sec
    _gnss_fix_interval = 3000; // 3000 msec
    _gnss_rx_timeout = 3000; // 3000 msec
    _gnss_retry_cnt = 1;
    _longitude = 0.0; // 126.944810;
    _latitude = 0.0; // 37.327204;

// _gnss_config
// 1 GPS ON/GLONASS ON/BeiDou OFF/Galileo OFF/QZSS OFF
// 2 GPS ON/GLONASS OFF/BeiDou ON/Galileo OFF/QZSS OFF
// 3 GPS ON/GLONASS OFF/BeiDou OFF/Galileo ON/QZSS OFF
// 4 GPS ON/GLONASS OFF/BeiDou OFF/Galileo OFF/QZSS ON
// 5 The constellation is selected based on MCC of camped network

    _field_test = 0;

    return RET_OK;
}

// =============================================================
// version
int config_manager::get_fw_name(string& fw_name)
{
    fw_name.append(FIRMWARE_NAME);
    log_d("%s %s\n", __func__, fw_name.c_str());
    return RET_OK;
}

int config_manager::get_sw_version(string& ver)
{
    char buf[32] = {0,};
    snprintf(buf, sizeof(buf), "%u.%u.%u", SW_VER_MAJOR, SW_VER_MINOR, SW_VER_BUILD);
    ver = buf;
    log_d("%s url[%s]\n", __func__, ver.c_str());
    return RET_OK;
}

// =============================================================
// at-handle
string& config_manager::get_at_dev_path(void)
{
    log_v("%s value[%s]\n", __func__, _at_dev_path.c_str());
    return _at_dev_path;
}

int config_manager::set_at_dev_path(string &value)
{
    _at_dev_path = value;
    log_v("%s value[%s]\n", __func__, _at_dev_path.c_str());
    return RET_OK;
}

u32 config_manager::get_at_buf_sz(void)
{
    log_v("%s value [%d] \n", __func__, _at_buf_sz);
    return _at_buf_sz;
}

int config_manager::set_at_buf_sz(u32 value)
{
    _at_buf_sz = value;
    log_v("%s value [%d] \n", __func__, _at_buf_sz);
    return RET_OK;
}

u32 config_manager::get_at_timeout(void)
{
    log_v("%s value [%d] \n", __func__, _at_timeout);
    return _at_timeout;
}

int config_manager::set_at_timeout(u32 value)
{
    _at_timeout = value;
    log_v("%s value [%d] \n", __func__, _at_timeout);
    return RET_OK;
}

int config_manager::get_at_in_progress(void)
{
    log_v("%s value [%d] \n", __func__, _at_in_progress);
    return _at_in_progress;
}

int config_manager::set_at_in_progress(int value)
{
    _at_in_progress = value;
    log_v("%s value [%d] \n", __func__, _at_in_progress);
    return RET_OK;
}

// =============================================================
// modem proc
u32 config_manager::get_mod_enable(void)
{
    log_v("%s value [%d] \n", __func__, _mod_enable);
    return _mod_enable;
}

int config_manager::set_mod_enable(u32 value)
{
    _mod_enable = value;
    log_v("%s value [%d] \n", __func__, _mod_enable);
    return RET_OK;
}

u32 config_manager::get_mod_timeout(void)
{
    log_v("%s value [%d] \n", __func__, _mod_timeout);
    return _mod_timeout;
}

int config_manager::set_mod_timeout(u32 value)
{
    _mod_timeout = value;
    log_v("%s value [%d] \n", __func__, _mod_timeout);
    return RET_OK;
}

u32 config_manager::get_mod_state(void)
{
    log_v("%s value [%d] \n", __func__, _mod_state);
    return _mod_state;
}

int config_manager::set_mod_state(u32 value)
{
    _mod_state = value;
    log_v("%s value [%d] \n", __func__, _mod_state);
    return RET_OK;
}

u32 config_manager::get_gnss_state(void)
{
    log_v("%s value [%d] \n", __func__, _gnss_state);
    return _gnss_state;
}

int config_manager::set_gnss_state(u32 value)
{
    _gnss_state = value;
    log_v("%s value [%d] \n", __func__, _gnss_state);
    return RET_OK;
}

string& config_manager::get_imei(void)
{
    log_v("%s value[%s]\n", __func__, _imei.c_str());
    return _imei;
}

int config_manager::set_imei(string &value)
{
    _imei = value;
    log_v("%s value[%s]\n", __func__, _imei.c_str());
    return RET_OK;
}

string& config_manager::get_revision(void)
{
    log_v("%s value[%s]\n", __func__, _revision.c_str());
    return _revision;
}

int config_manager::set_revison(string &value)
{
    _revision = value;
    log_v("%s value[%s]\n", __func__, _revision.c_str());
    return RET_OK;
}

string& config_manager::get_version(void)
{
    log_v("%s value[%s]\n", __func__, _version.c_str());
    return _version;
}

int config_manager::set_version(string &value)
{
    _version = value;
    log_v("%s value[%s]\n", __func__, _version.c_str());
    return RET_OK;
}

string& config_manager::get_imsi(void)
{
    log_v("%s value[%s]\n", __func__, _imsi.c_str());
    return _imsi;
}

int config_manager::set_imsi(string &value)
{
    _imsi = value;
    log_v("%s value[%s]\n", __func__, _imsi.c_str());
    return RET_OK;
}

string& config_manager::get_cpin(void)
{
    log_v("%s value[%s]\n", __func__, _cpin.c_str());
    return _cpin;
}

int config_manager::set_cpin(string &value)
{
    _cpin = value;
    log_v("%s value[%s]\n", __func__, _cpin.c_str());
    return RET_OK;
}

string& config_manager::get_qccid(void)
{
    log_v("%s value[%s]\n", __func__, _qccid.c_str());
    return _qccid;
}

int config_manager::set_qccid(string &value)
{
    _qccid = value;
    log_v("%s value[%s]\n", __func__, _qccid.c_str());
    return RET_OK;
}

string& config_manager::get_net_operator(void)
{
    log_v("%s value[%s]\n", __func__, _net_operator.c_str());
    return _net_operator;
}

int config_manager::set_net_operator(string &value)
{
    _net_operator = value;
    log_v("%s value[%s]\n", __func__, _net_operator.c_str());
    return RET_OK;
}

string& config_manager::get_access_technology(void)
{
    log_v("%s value[%s]\n", __func__, _access_technology.c_str());
    return _access_technology;
}

int config_manager::set_access_technology(string &value)
{
    _access_technology = value;
    log_v("%s value[%s]\n", __func__, _access_technology.c_str());
    return RET_OK;
}

string& config_manager::get_operator_number(void)
{
    log_v("%s value[%s]\n", __func__, _operator_num.c_str());
    return _operator_num;
}

int config_manager::set_operator_number(string &value)
{
    _operator_num = value;
    log_v("%s value[%s]\n", __func__, _operator_num.c_str());
    return RET_OK;
}

string& config_manager::get_band(void)
{
    log_v("%s value[%s]\n", __func__, _band.c_str());
    return _band;
}

int config_manager::set_band(string &value)
{
    _band = value;
    log_v("%s value[%s]\n", __func__, _band.c_str());
    return RET_OK;
}

u32 config_manager::get_channel(void)
{
    log_v("%s value [%d] \n", __func__, _channel);
    return _channel;
}

int config_manager::set_channel(u32 value)
{
    _channel = value;
    log_v("%s value [%d] \n", __func__, _channel);
    return RET_OK;
}

string& config_manager::get_sys_mode(void)
{
    log_v("%s value[%s]\n", __func__, _sys_mode.c_str());
    return _sys_mode;
}

int config_manager::set_sys_mode(string &value)
{
    _sys_mode = value;
    log_v("%s value[%s]\n", __func__, _sys_mode.c_str());
    return RET_OK;
}

s32 config_manager::get_rssi(void)
{
    log_v("%s value [%d] \n", __func__, _rssi);
    return _rssi;
}

int config_manager::set_rssi(s32 value)
{
    _rssi = value;
    log_v("%s value [%d] \n", __func__, _rssi);
    return RET_OK;
}

s32 config_manager::get_rsrp(void)
{
    log_v("%s value [%d] \n", __func__, _rsrp);
    return _rsrp;
}

int config_manager::set_rsrp(s32 value)
{
    _rsrp = value;
    log_v("%s value [%d] \n", __func__, _rsrp);
    return RET_OK;
}

s32 config_manager::get_sinr(void)
{
    log_v("%s value [%d] \n", __func__, _sinr);
    return _sinr;
}

int config_manager::set_sinr(s32 value)
{
    _sinr = value;
    log_v("%s value [%d] \n", __func__, _sinr);
    return RET_OK;
}

s32 config_manager::get_rsrq(void)
{
    log_v("%s value [%d] \n", __func__, _rsrq);
    return _rsrq;
}

int config_manager::set_rsrq(s32 value)
{
    _rsrq = value;
    log_v("%s value [%d] \n", __func__, _rsrq);
    return RET_OK;
}

string& config_manager::get_fnn(void)
{
    log_v("%s value[%s]\n", __func__, _fnn.c_str());
    return _fnn;
}

int config_manager::set_fnn(string &value)
{
    _fnn = value;
    log_v("%s value[%s]\n", __func__, _fnn.c_str());
    return RET_OK;
}

string& config_manager::get_snn(void)
{
    log_v("%s value[%s]\n", __func__, _snn.c_str());
    return _snn;
}

int config_manager::set_snn(string &value)
{
    _snn = value;
    log_v("%s value[%s]\n", __func__, _snn.c_str());
    return RET_OK;
}

string& config_manager::get_spn(void)
{
    log_v("%s value[%s]\n", __func__, _spn.c_str());
    return _spn;
}

int config_manager::set_spn(string &value)
{
    _spn = value;
    log_v("%s value[%s]\n", __func__, _spn.c_str());
    return RET_OK;
}

string& config_manager::get_rplwn(void)
{
    log_v("%s value[%s]\n", __func__, _rplwn.c_str());
    return _rplwn;
}

int config_manager::set_rplwn(string &value)
{
    _rplwn = value;
    log_v("%s value[%s]\n", __func__, _rplwn.c_str());
    return RET_OK;
}

string& config_manager::get_pdp_type(void)
{
    log_v("%s value[%s]\n", __func__, _pdp_type.c_str());
    return _pdp_type;
}

int config_manager::set_pdp_type(string &value)
{
    _pdp_type = value;
    log_v("%s value[%s]\n", __func__, _pdp_type.c_str());
    return RET_OK;
}

string& config_manager::get_apn(void)
{
    log_v("%s value[%s]\n", __func__, _apn.c_str());
    return _apn;
}

int config_manager::set_apn(string &value)
{
    _apn = value;
    log_v("%s value[%s]\n", __func__, _apn.c_str());
    return RET_OK;
}

string& config_manager::get_pdp_addr(void)
{
    log_v("%s value[%s]\n", __func__, _pdp_addr.c_str());
    return _pdp_addr;
}

int config_manager::set_pdp_addr(string &value)
{
    _pdp_addr = value;
    log_v("%s value[%s]\n", __func__, _pdp_addr.c_str());
    return RET_OK;
}

string& config_manager::get_cclk(void)
{
    log_v("%s value[%s]\n", __func__, _cclk.c_str());
    return _cclk;
}

int config_manager::set_cclk(string &value)
{
    _cclk = value;
    log_v("%s value[%s]\n", __func__, _cclk.c_str());
    return RET_OK;
}

string& config_manager::get_usim(void)
{
    log_v("%s value[%s]\n", __func__, _usim.c_str());
    return _usim;
}

int config_manager::set_usim(string &value)
{
    _usim = value;
    log_v("%s value[%s]\n", __func__, _usim.c_str());
    return RET_OK;
}

string& config_manager::get_number(void)
{
    log_v("%s value[%s]\n", __func__, _number.c_str());
    return _number;
}

int config_manager::set_number(string &value)
{
    _number = value;
    log_v("%s value[%s]\n", __func__, _number.c_str());
    return RET_OK;
}

// =============================================================
// location
u32 config_manager::get_gnss_enable(void)
{
    log_v("%s value [%d] \n", __func__, _gnss_enable);
    return _gnss_enable;
}

int config_manager::set_gnss_enable(u32 value)
{
    _gnss_enable = value;
    log_i("%s value [%d] \n", __func__, _gnss_enable);
    return RET_OK;
}

u32 config_manager::get_gnss_config(void)
{
    log_v("%s value [%d] \n", __func__, _gnss_config);
    return _gnss_config;
}

int config_manager::set_gnss_config(u32 value)
{
    _gnss_config = value;
    log_i("%s value [%d] \n", __func__, _gnss_config);
    return RET_OK;
}

u32 config_manager::get_gnss_interval(void)
{
    log_v("%s value [%d] msec\n", __func__, _gnss_interval);
    return _gnss_interval;
}

int config_manager::set_gnss_interval(u32 value)
{
    _gnss_interval = value;
    log_i("%s value [%d] msec\n", __func__, _gnss_interval);
    return RET_OK;
}

u32 config_manager::get_gnss_fix_timeout(void)
{
    log_v("%s value [%d] msec\n", __func__, _gnss_fix_timeout);
    return _gnss_fix_timeout;
}

int config_manager::set_gnss_fix_timeout(u32 value)
{
    _gnss_fix_timeout = value;
    log_i("%s value [%d] msec\n", __func__, _gnss_fix_timeout);
    return RET_OK;
}

u32 config_manager::get_gnss_fix_interval(void)
{
    log_v("%s value [%d] msec\n", __func__, _gnss_fix_interval);
    return _gnss_fix_interval;
}

int config_manager::set_gnss_fix_interval(u32 value)
{
    _gnss_fix_interval = value;
    log_i("%s value [%d] msec\n", __func__, _gnss_fix_interval);
    return RET_OK;
}

u32 config_manager::get_gnss_rx_timeout(void)
{
    log_v("%s value [%d] msec\n", __func__, _gnss_rx_timeout);
    return _gnss_rx_timeout;
}

int config_manager::set_gnss_rx_timeout(u32 value)
{
    _gnss_rx_timeout = value;
    log_i("%s value [%d] msec\n", __func__, _gnss_rx_timeout);
    return RET_OK;
}

u32 config_manager::get_gnss_retry_cnt(void)
{
    log_v("%s value [%d]\n", __func__, _gnss_retry_cnt);
    return _gnss_retry_cnt;
}

int config_manager::set_gnss_retry_cnt(u32 value)
{
    _gnss_retry_cnt = value;
    log_i("%s value [%d] msec\n", __func__, _gnss_retry_cnt);
    return RET_OK;
}

double config_manager::get_longitude(void)
{
    log_v("%s value[%lf]\n", __func__, _longitude);
    return _longitude;
}

int config_manager::set_longitude(double value)
{
    _longitude = value;
    log_v("%s value[%lf]\n", __func__, _longitude);
    return RET_OK;
}

double config_manager::get_latitude(void)
{
    log_v("%s value[%lf]\n", __func__, _latitude);
    return _latitude;
}

int config_manager::set_latitude(double value)
{
    _latitude = value;
    log_v("%s value[%lf]\n", __func__, _latitude);
    return RET_OK;
}

u32 config_manager::get_field_test(void)
{
    log_v("%s value [%d] \n", __func__, _field_test);
    return _field_test;
}

int config_manager::set_field_test(u32 value)
{
    _field_test = value;
    log_v("%s value [%d] \n", __func__, _field_test);
    return RET_OK;
}

