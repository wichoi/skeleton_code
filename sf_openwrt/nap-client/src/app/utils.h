#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

#include "common.h"

#define INTERFACE_WAN       "eth1"
#define INTERFACE_LAN       "eth0"
#define INTERFACE_LTE       "ppp0"
#define INTERFACE_STA       "apcli0" // Wifi Station Mode

#define INTERFACE_IPV4      "IPv4"
#define INTERFACE_IPV6      "IPv6"

class utils
{
private:
    utils();
    ~utils();

public:
    static int read_conn_interface(string &ifname);
    static int read_conn_type(string &conn_type);
    static int read_ip_type(string &ip_type);
    static int read_ip_addr(string &ip_addr, string &ip_type, string &ifname);
    static u32 read_eth1_tx(void);
    static u32 read_eth1_rx(void);
    static u32 read_ppp0_tx(void);
    static u32 read_ppp0_rx(void);
    static u32 read_apcli0_tx(void);
    static u32 read_apcli0_rx(void);
    static int read_mac(string &mac, string ifname = INTERFACE_WAN);
    static int read_link_state(bool &link_st, string &ifname);
    static int read_vlan_table(bool &valid);
    static int read_interface_state(bool &if_st, string &ifname);
    static int read_expiration_time(string &time, int exp = 0);
    static int read_timestamp(string &timestamp);
    static int read_unix_timestamp(u32 &timestamp);
    static int convert_timestamp(s64 src_time, string &dest_time);
    static s64 get_local_time(void);
    static int set_local_time(s64 utc_time);
    static void utc_to_gpstime (int32_t *wn, int32_t *tow);
    static int get_utc_time(int &year, int &month, int &day, int &hour, int &min, int &sec);
    static u32 read_uptime(void);
    static int read_cpu_usage(string &cpu);
    static int read_mem_usage(string &mem);
    static int read_version(string &ver);
    static int system_reboot(void);
    static int system_factory_reset(void);
    static int system_call(string &cmd, string &result);
    static int system_call(string &cmd);
    static u32 file_size(string file_path);
    static int nvram_ontime_init(void);
    static int nvram_uboot_get(string &key, string &value);
    static int nvram_uboot_set(string &key, string &value);
    static int nvram_rt2860_get(string &key, string &value);
    static int nvram_rt2860_set(string &key, string &value);
    static int nvram_rt2860_commit(void);
    static int modem_gps_updating_check(void);
    static int modem_gps_get(double &longitude, double &latitude);
    static int modem_conf_get(string key, int &value);
    static int modem_conf_get(string key, double &value);
    static int modem_conf_get(string key, string &value);
    static int hex_printf(char *data, u8 len);
    static int dump_log_file(string &value);
    static int udhcpc_renewal(void);
};

#endif
