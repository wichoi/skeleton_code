#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

#include "common.h"

class utils
{
private:
    utils();
    ~utils();

public:
    static int read_mac(string &mac);
    static int read_ip_addr(string &ip_addr);
    static int read_ip_type(string &ip_type);
    static int read_conn_type(string &conn_type);
    static int read_current_time(string &time);
    static int read_timestamp(string &ev_id);
    static u32 read_uptime(void);
    static u32 gps_updating_create(void);
    static u32 gps_updating_remove(void);
};

#endif
