#ifndef __AT_CONFIG_H__
#define __AT_CONFIG_H__

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
// version
    int get_fw_name(string& fw_name);
    int get_sw_version(string& ver);

// =============================================================
// at-handle
    string& get_at_dev_path(void);
    int set_at_dev_path(string &value);

    u32 get_at_buf_sz(void);
    int set_at_buf_sz(u32 value);

    u32 get_at_timeout(void);
    int set_at_timeout(u32 value);

    int get_at_in_progress(void);
    int set_at_in_progress(int value);

// =============================================================
// modem proc
    u32 get_mod_enable(void);
    int set_mod_enable(u32 value);

    u32 get_mod_timeout(void);
    int set_mod_timeout(u32 value);

    u32 get_mod_state(void);
    int set_mod_state(u32 value);

    u32 get_gnss_state(void);
    int set_gnss_state(u32 value);

    string& get_imei(void);
    int set_imei(string &value);

    string& get_revision(void);
    int set_revison(string &value);

    string& get_version(void);
    int set_version(string &value);

    string& get_imsi(void);
    int set_imsi(string &value);

    string& get_cpin(void);
    int set_cpin(string &value);

    string& get_qccid(void);
    int set_qccid(string &value);

    string& get_net_operator(void);
    int set_net_operator(string &value);

    string& get_access_technology(void);
    int set_access_technology(string &value);

    string& get_operator_number(void);
    int set_operator_number(string &value);

    string& get_band(void);
    int set_band(string &value);

    u32 get_channel(void);
    int set_channel(u32 value);

    string& get_sys_mode(void);
    int set_sys_mode(string &value);

    s32 get_rssi(void);
    int set_rssi(s32 value);

    s32 get_rsrp(void);
    int set_rsrp(s32 value);

    s32 get_sinr(void);
    int set_sinr(s32 value);

    s32 get_rsrq(void);
    int set_rsrq(s32 value);

    string& get_fnn(void);
    int set_fnn(string &value);

    string& get_snn(void);
    int set_snn(string &value);

    string& get_spn(void);
    int set_spn(string &value);

    string& get_rplwn(void);
    int set_rplwn(string &value);

    string& get_pdp_type(void);
    int set_pdp_type(string &value);

    string& get_apn(void);
    int set_apn(string &value);

    string& get_pdp_addr(void);
    int set_pdp_addr(string &value);

    string& get_cclk(void);
    int set_cclk(string &value);

    string& get_usim(void);
    int set_usim(string &value);

    string& get_number(void);
    int set_number(string &value);

// =============================================================
// location
    u32 get_gnss_enable(void);
    int set_gnss_enable(u32 value);

    u32 get_gnss_config(void);
    int set_gnss_config(u32 value);

    u32 get_gnss_interval(void);
    int set_gnss_interval(u32 value);

    u32 get_gnss_fix_timeout(void);
    int set_gnss_fix_timeout(u32 value);

    u32 get_gnss_fix_interval(void);
    int set_gnss_fix_interval(u32 value);

    u32 get_gnss_rx_timeout(void);
    int set_gnss_rx_timeout(u32 value);

    u32 get_gnss_retry_cnt(void);
    int set_gnss_retry_cnt(u32 value);

    double get_longitude(void);
    int set_longitude(double value);

    double get_latitude(void);
    int set_latitude(double value);

    u32 get_field_test(void);
    int set_field_test(u32 value);

private:
    static config_manager *_singleton_instance;

    // at-handle
    string _at_dev_path;
    u32 _at_buf_sz;
    u32 _at_timeout;
    int _at_in_progress;

    // modem proc
    u32 _mod_enable;
    u32 _mod_timeout;
    u32 _mod_state;
    u32 _gnss_state;
    string _imei;
    string _revision;
    string _version;
    string _imsi;
    string _cpin;
    string _qccid;
    string _net_operator;
    string _access_technology;
    string _operator_num;
    string _band;
    u32 _channel;
    string _sys_mode;
    s32 _rssi;
    s32 _rsrp;
    s32 _sinr;
    s32 _rsrq;
    string _fnn;
    string _snn;
    string _spn;
    string _rplwn;
    string _pdp_type;
    string _apn;
    string _pdp_addr;
    string _cclk;
    string _usim;
    string _number;

    // location
    u32 _gnss_enable;
    u32 _gnss_config;
    u32 _gnss_interval; // msec
    u32 _gnss_fix_timeout;
    u32 _gnss_fix_interval;
    u32 _gnss_rx_timeout;
    u32 _gnss_retry_cnt;
    double _longitude;
    double _latitude;

    u32 _field_test;
};

#endif
