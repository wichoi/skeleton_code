#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <chrono>

#include "log.h"
#include "utils.h"

using namespace chrono;

//using namespace chrono;
//using std::cout; 
//using std::endl;
//using std::chrono::duration_cast;
//using std::chrono::milliseconds;
//using std::chrono::seconds;
//using std::chrono::system_clock;

utils::utils()
{
}

utils::~utils()
{
}

int utils::read_mac(string &mac)
{
    mac = "00:1F:96:28:80:2D";
    log_d("%s [%s]\n", __func__, mac.c_str());
    return RET_OK;
}

int utils::read_ip_addr(string &ip_addr)
{
    ip_addr = "192.168.0.39";
    log_d("%s [%s]\n", __func__, ip_addr.c_str());
    return RET_OK;
}

int utils::read_ip_type(string &ip_type)
{
    ip_type = "IPv4";
    log_d("%s [%s]\n", __func__, ip_type.c_str());
    return RET_OK;
}

int utils::read_conn_type(string &conn_type)
{
    conn_type = "WAN";
    log_d("%s [%s]\n", __func__, conn_type.c_str());
    return RET_OK;
}

int utils::read_current_time(string &time)
{
    char time_str[32] = {0,};

    system_clock::time_point currTimePoint = system_clock::now();
    time_t currTime = system_clock::to_time_t(currTimePoint);
    snprintf(time_str, sizeof(time_str), "%ld", currTime);
    time = time_str;
    log_d("%s [%s]\n", __func__, time.c_str());
    return RET_OK;
}

int utils::read_timestamp(string &ev_id)
{
    system_clock::time_point now_time = system_clock::now();
    time_t curr_time = system_clock::to_time_t(now_time);
    struct tm *t = localtime(&curr_time);
    //milliseconds curr_msec = duration_cast<milliseconds>(now_time.time_since_epoch());
    struct timeval tv;
    gettimeofday(&tv, NULL);

    char date_time[128] = { 0x0, };
    snprintf(date_time, sizeof(date_time),
        "%04d-%02d-%02d-%02d:%02d:%02d.%03lu",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec,
        tv.tv_usec / 1000);
    ev_id = date_time;

    log_d("%s [%s]\n", __func__, ev_id.c_str());
    return RET_OK;
}

u32 utils::read_uptime(void)
{
    u32 uptime;
    FILE* fp = fopen("/proc/uptime", "r");
    if(fp == NULL) return 0;
    fscanf(fp, "%u", &uptime);
    log_d("%s uptime[%u]\n", __func__, uptime);
    fclose(fp);
    return uptime;
}

u32 utils::gps_updating_create(void)
{
    u32 ret_val = RET_ERROR;
    FILE* fp = fopen("/tmp/GPSupdating", "w");
    if(fp != NULL)
    {
        ret_val = RET_OK;
        fclose(fp);
    }
    return ret_val;
}

u32 utils::gps_updating_remove(void)
{
    u32 ret_val = RET_OK;
    if (access("/tmp/GPSupdating", F_OK) == 0)
    {
        remove("/tmp/GPSupdating");
    }
    return ret_val;
}

