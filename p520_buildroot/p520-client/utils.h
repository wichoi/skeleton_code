#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <list>

#include "common.h"

class utils
{
private:
    utils();
    ~utils();

public:
    static u32 read_ip_addr(string &ip_addr);
    static u32 read_link_state(bool &link_st);
    static u32 read_interface_state(bool &if_st);
    static u32 read_uptime(void);
    static u32 read_random_time(string &start_tm, u32 span);
    static int read_timestamp(string &timestamp);
    static int update_timestamp(void);
    static int read_cpu_usage(string &cpu);
    static int read_mem_usage(string &mem);
    static int system_reboot(void);
    static int system_call(string &cmd, string &result);
    static int system_call(string &cmd);
    static int file_size(string &file_path);
    static int dump_log_file(string &value);
    static int at_command(string &atcmd, string &result);

    static int nvram_get_2860(string &key, string &value);
    static int nvram_set_2860(string &key, string &value);

    static int http_header_parser(string &header);

    static int set_modem_band(string &band);
    static int set_modem_apn(string &apn);

    static int get_modem_sms(string &sms_body, int index);
    static int rm_modem_sms(void);
    static int modem_sms_command(int &mdm_reset, int &mdm_alive);

    static int update_modem_ati(void);
    static int update_modem_cnum(void);
    static int update_modem_serial(void);
    static int update_modem_sysinfo(void);
    static int update_modem_cgpaddr(void);
    static int update_modem_cgdcont(void);
    static int update_modem_bandchange(void);
    static int update_modem_swver(void);
    static int update_modem_cclk(int &y, int &m, int &d, int &h, int &min, int &s);
    static int update_modem_info(void);

    static int update_router_version(void);
    static int update_router_usb0_tx(void);
    static int update_router_usb0_rx(void);
    static int update_router_eth2_tx(void);
    static int update_router_eth2_rx(void);
    static int update_router_info(void);

    static int encode_base64(string& dest, const string& src);
    static int decode_base64(string& dest, const string& src);
    static int encode_base64(string& dest, const unsigned char *src, int size);
    static int decode_base64(char *dest, int *len, const string& src);

    static bool hex_to_char(char const* szHex, unsigned char* rch);
    static int hex_to_string(string &dest, string &src);

    static int cloud_hostbyname(string &url, list<string> &ip_list);
};

#endif
