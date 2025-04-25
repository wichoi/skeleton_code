#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <chrono>
#include <algorithm>
#include <string>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/wireless.h>

#include "json/cJSON.h"

#include "log.h"
#include "utils.h"
#include "config-manager.h"

#ifdef LINUX_PC_APP
#else
  #include <pthread.h>
  #include "nvram.h"
  #include "switch/switch_netlink.h"
  static pthread_mutex_t _mutex_lock;
#endif

#define BACKHAUE_WAN        "WAN"
#define BACKHAUL_LTE        "LTE"
#define BACKHAUL_WIFI       "WIFI"

using namespace chrono;

struct in6_ifreq
{
    struct in6_addr ifr6_addr;
    uint32_t        ifr6_prefixlen;
    int             ifr6_ifindex;
};

utils::utils()
{
#ifdef LINUX_PC_APP
#else
    pthread_mutex_init(&_mutex_lock,NULL);
#endif
}

utils::~utils()
{
#ifdef LINUX_PC_APP
#else
    pthread_mutex_destroy(&_mutex_lock);
#endif
}

int utils::read_conn_interface(string &ifname)
{
#ifdef LINUX_PC_APP
    ifname = INTERFACE_WAN;
#else
    FILE *fp;
    u32 index;
    char type_str[32] ={0,};
    if((fp = fopen("/etc/wan", "r")) == NULL)
    {
        log_e("failed to open /etc/wan\n");
        return RET_ERROR;
    }
    fscanf(fp, "%d %s", &index, type_str);
    fclose(fp);
    ifname = type_str; // eth1, ppp0, apcli0
#endif
    log_d("%s [%s]\n", __func__, ifname.c_str());
    return RET_OK;
}

int utils::read_conn_type(string &conn_type)
{
#ifdef LINUX_PC_APP
    conn_type = BACKHAUE_WAN;
#else
    string ifname = "";
    utils::read_conn_interface(ifname);
    if(ifname.compare(INTERFACE_WAN) == 0)
        conn_type = BACKHAUE_WAN;
    else if(ifname.compare(INTERFACE_LTE) == 0)
        conn_type = BACKHAUL_LTE;
    else if(ifname.compare(INTERFACE_STA) == 0)
        conn_type = BACKHAUL_WIFI;
    else
        conn_type = BACKHAUE_WAN;
#endif
    log_d("%s [%s]\n", __func__, conn_type.c_str());
    return RET_OK;
}

int utils::read_ip_type(string &ip_type)
{
#ifdef LINUX_PC_APP
    ip_type = INTERFACE_IPV4;
#else
    ip_type = INTERFACE_IPV4;
#if 0 // todo
    FILE *fp;
    char type_str[32] ={0,};
    if((fp = fopen("/etc/ip-type", "r")) == NULL)
    {
        log_e("failed to open /etc/ip-type\n");
        return RET_ERROR;
    }
    fscanf(fp, "%s", type_str);
    fclose(fp);

    if(strncmp((const char*)type_str, INTERFACE_IPV4, strlen(INTERFACE_IPV4)) == 0)
        conn_type = "IPv4";
    else if(strncmp((const char*)type_str, INTERFACE_IPV6, strlen(INTERFACE_IPV6)) == 0)
        conn_type = "IPv6";
    else
        conn_type = "IPv4";
#endif
#endif
    log_d("%s [%s]\n", __func__, ip_type.c_str());
    return RET_OK;
}

int utils::read_ip_addr(string &ip_addr, string &ip_type, string &ifname)
{
#ifdef LINUX_PC_APP
    ip_addr = "192.168.0.39";
#else
    if(ip_type.compare(INTERFACE_IPV6) == 0)
    {
        struct in6_ifreq ifr;
        struct sockaddr_in6 *sin = (struct sockaddr_in6 *) &ifr.ifr6_addr;
        int skfd = 0;
        if((skfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0)
        {
            log_e("open socket error\n");
            return RET_ERROR;
        }

        memset(&ifr, 0, sizeof ifr);
        sin->sin6_family = AF_INET6;
        if(ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
        {
            close(skfd);
            log_v("ioctl SIOCGIFADDR error for %s\n", ifname.c_str());
            return RET_ERROR;
        }

        char str[INET6_ADDRSTRLEN] = {0,};
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sin)->sin6_addr), str, INET6_ADDRSTRLEN);
        ip_addr.append(str);
        close(skfd);
    }
    else
    {
        struct ifreq ifr;
        int skfd = 0;
        if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            log_e("open socket error\n");
            return RET_ERROR;
        }

        strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ);
        if(ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
        {
            close(skfd);
            log_v("ioctl SIOCGIFADDR error for %s\n", ifname.c_str());
            return RET_ERROR;
        }

        ip_addr = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
        close(skfd);
    }
#endif
    log_v("%s [%s]\n", __func__, ip_addr.c_str());
    return RET_OK;
}

u32 utils::read_eth1_tx(void)
{
    u32 data = 0;
    string path = "/sys/class/net/eth1/statistics/tx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
    }
    log_d("%s %d\n", __func__, data);
    return data;
}

u32 utils::read_eth1_rx(void)
{
    u32 data = 0;
    string path = "/sys/class/net/eth1/statistics/rx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
    }
    log_d("%s %d\n", __func__, data);
    return data;
}

u32 utils::read_ppp0_tx(void)
{
    u32 data = 0;
    string path = "/sys/class/net/ppp0/statistics/tx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
    }
    log_d("%s %d\n", __func__, data);
    return data;
}

u32 utils::read_ppp0_rx(void)
{
    u32 data = 0;
    string path = "/sys/class/net/ppp0/statistics/rx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
    }
    log_d("%s %d\n", __func__, data);
    return data;
}

u32 utils::read_apcli0_tx(void)
{
    u32 data = 0;
    string path = "/sys/class/net/apcli0/statistics/tx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
    }
    log_d("%s %d\n", __func__, data);
    return data;
}

u32 utils::read_apcli0_rx(void)
{
    u32 data = 0;
    string path = "/sys/class/net/apcli0/statistics/rx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
    }
    log_d("%s %d\n", __func__, data);
    return data;
}

int utils::read_mac(string &mac, string ifname)
{
#ifdef LINUX_PC_APP
    mac = "00:1F:96:28:80:2D";
    log_d("mac[%s]\n", mac.c_str());
#else
    struct ifreq ifr;
    char *ptr;
    char mac_buf[32] = {0,};
    int skfd;

    if ((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        log_e("open socket error\n");
        return RET_ERROR;
    }

    strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ);
    if (ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0)
    {
        close(skfd);
        log_v("ioctl SIOCGIFHWADDR error for %s\n", ifname.c_str());
        return RET_ERROR;
    }

    ptr = (char *)&ifr.ifr_addr.sa_data;
    snprintf(mac_buf, sizeof(mac_buf), "%02X:%02X:%02X:%02X:%02X:%02X",
                    (ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
                    (ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));
    close(skfd);
    mac = mac_buf;
    log_d("%s mac[%s]\n", ifname.c_str(), mac.c_str());
#endif
    return RET_OK;
}

int utils::read_link_state(bool &link_st, string &ifname)
{
    int ret_val = RET_ERROR;
#ifdef LINUX_PC_APP
    link_st = true;
    ret_val = RET_OK;
#else
    if(ifname.compare(INTERFACE_WAN) == 0)
    {
        pthread_mutex_lock(&_mutex_lock);
        if(mt753x_netlink_init() == 0)
        {
            int value = 0;
            struct mt753x_attr attres = {0,};
            if (phy_cl22_read_netlink(&attres, -1, 0x3008, &value) < 0)
            {
                log_v("%s Read fail\n", __func__);
            }

            //log_i("%s value[0x%02X]\n", __func__, value);
            if(value & 0x0001)
            {
                link_st = true;
            }
            else
            {
                link_st = false;
            }
        }

        mt753x_netlink_free();
        pthread_mutex_unlock(&_mutex_lock);
    }
    else
    {
        link_st = true;
        ret_val = RET_OK;
    }
#endif
    log_v("%s link_st[%d]\n", __func__, link_st);
    return ret_val;
}

int utils::read_vlan_table(bool &valid)
{
    int ret_val = RET_ERROR;
#ifdef LINUX_PC_APP
    link_st = true;
    ret_val = RET_OK;
#else
    pthread_mutex_lock(&_mutex_lock);
    valid = false;
    if(mt753x_netlink_init() == 0)
    {
        int offset = REG_VTCR_ADDR, value, j;
        struct mt753x_attr attres = {0,};
        if (reg_write_netlink(&attres, offset, (0x80000000 + 1)) < 0)
		{
             log_v("%s Write fail\n", __func__);
             mt753x_netlink_free();
             pthread_mutex_unlock(&_mutex_lock);
			return ret_val;
		}

        offset = REG_VTCR_ADDR;
        for (j = 0; j < 20; j++)
        {
            if(reg_read_netlink(&attres, offset, &value) < 0)
            {
                usleep(1000);
				continue;
            }
            else if ((value & 0x80000000) == 0) { //mac address busy
                break;
            }
            usleep(1000);
        }
        if (j == 20)
		{
             log_v("%s read fail\n", __func__);
             mt753x_netlink_free();
             pthread_mutex_unlock(&_mutex_lock);
		  	 return ret_val;
		}

        offset = REG_VAWD1_ADDR;
        if(reg_read_netlink(&attres, offset, &value) < 0)
        {
             log_v("%s read fail 2\n", __func__);
             mt753x_netlink_free();
             pthread_mutex_unlock(&_mutex_lock);
		  	 return ret_val;
        }

		if ((value & 0x01) != 0)
		    if ((value & 0x00ff0000) == 0x005e0000)	// portmap -1111-1-
                valid = true;
        ret_val = RET_OK;
    }

    mt753x_netlink_free();
    pthread_mutex_unlock(&_mutex_lock);
#endif
    log_v("%s vid[1] valid[%d]\n", __func__, valid);
    return ret_val;
}

int utils::read_interface_state(bool &if_st, string &ifname)
{
    int ret_val = RET_ERROR;
#ifdef LINUX_PC_APP
    if_st = true;
    ret_val = RET_OK;
#else
    struct ifreq ifr;
    int skfd;
    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd == -1)
    {
        log_d("%s socket error\n", __func__);
        return ret_val;
    }
    strncpy(ifr.ifr_name, ifname.c_str(), sizeof(ifr.ifr_name));
    if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
    {
        log_v("ioctl SIOCGIFFLAGS error for %s\n", ifname.c_str());
        close(skfd);
        return ret_val;
    }
    close(skfd);

    if (ifr.ifr_flags & IFF_UP)
        if_st = true;
    else
        if_st = false;

    ret_val = RET_OK;
#endif
    log_v("%s if_st[%d]\n", __func__, if_st);
    return ret_val;
}

int utils::read_expiration_time(string &time, int exp)
{
    char time_str[32] = {0,};
    // std::chrono::milliseconds(exp);
    system_clock::time_point timeout = system_clock::now() + seconds(exp);
    time_t currTime = system_clock::to_time_t(timeout);
    snprintf(time_str, sizeof(time_str), "%ld", currTime);
    time = time_str;
    log_d("%s [%s]\n", __func__, time.c_str());
    return RET_OK;
}

int utils::read_timestamp(string &timestamp)
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
    timestamp = date_time;

    log_d("%s [%s]\n", __func__, timestamp.c_str());
    return RET_OK;
}

int utils::read_unix_timestamp(u32 &timestamp)
{
    system_clock::time_point now_time = system_clock::now();
    timestamp = system_clock::to_time_t(now_time);
    return RET_OK;
}

int utils::convert_timestamp(s64 src_time, string &dest_time)
{
    time_t curr_time = (time_t)src_time;
    struct tm *t = localtime(&curr_time);
    char time_buf[128] = { 0x0, };
    snprintf(time_buf, sizeof(time_buf),
        "%04d-%02d-%02d %02d:%02d:%02d",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
    dest_time = time_buf;
    return RET_OK;
}

s64 utils::get_local_time(void)
{
    s64 ret_val = 0;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ret_val = (s64)tv.tv_sec * 1000 +  tv.tv_usec % 1000;
    log_i("%s [%llu]\n", __func__, ret_val);
    return ret_val;
}

int utils::set_local_time(s64 utc_time)
{
    time_t local_time = time(NULL);
    struct timeval tv;
    tv.tv_sec = utc_time / 1000;
    tv.tv_usec = (utc_time % 1000) * 1000;

    log_i("%s locat_time[%u] utc_time[%llu] tv_sec[%u] tv_usec[%u]\n",
                    __func__, local_time, utc_time, tv.tv_sec, tv.tv_usec);
    if(local_time < tv.tv_sec - 10 || local_time > tv.tv_sec + 10)
    {
        log_w("time mismatch utc_time set to local_time !!!\n");
        settimeofday(&tv, NULL);
    }

    return RET_OK;
}

void utils::utc_to_gpstime(int32_t *wn, int32_t *tow) // GPS week number,GPS time of week
{
    system_clock::time_point now_time = system_clock::now();
    time_t curr_time = system_clock::to_time_t(now_time);
    struct tm *t = localtime(&curr_time);
    uint32_t year = t->tm_year + 1900;
    uint8_t mon = t->tm_mon + 1;
    uint8_t day = t->tm_mday;
    uint8_t hour = t->tm_hour;
    uint8_t min = t->tm_min;
    uint8_t sec = t->tm_sec;

    int32_t iYearsElapsed; //Elapsed years since 1980
    int32_t iDaysElapsed; //Elapsed days since Jan 5/Jan 6, 1980
    int32_t iLeapDays; //Leap days since Jan 5/Jan 6, 1980
    int32_t i;

    //Number of days at the start of each month (ignore leap years).
    uint16_t doy[12]={0,31,59,90,120,151,181,212,243,273,304,334};

    iYearsElapsed = year - 1980;
    i = 0;
    iLeapDays =0;
    while(i <=iYearsElapsed)
    {
        if((i % 100) == 20)
        {
            if((i % 400) == 20)
            {
                iLeapDays++;
            }
        }
        else if((i % 4) == 0)
        {
            iLeapDays++;
        }
        i++;
    }
    /*  iLeapDays = iYearsElapsed / 4 + 1; */
    if((iYearsElapsed % 100) == 20)
    {
        if(((iYearsElapsed % 400) == 20) && (mon <= 2))
        {
            iLeapDays--;
        }
    }
    else if(((iYearsElapsed % 4) == 0) && (mon <= 2))
    {
        iLeapDays--;
    }
    iDaysElapsed = iYearsElapsed * 365 + doy[mon -1] + day + iLeapDays - 6;
    //Convert time to GPS weeks and seconds.
    *wn = iDaysElapsed / 7;
    *tow =((iDaysElapsed % 7) * 86400) + (hour * 3600 + min * 60) + sec;
}

int utils::get_utc_time(int &year, int &month, int &day, int &hour, int &min, int &sec)
{
    system_clock::time_point now_time = system_clock::now();
    time_t curr_time = system_clock::to_time_t(now_time);
    struct tm *t = localtime(&curr_time);
    year = t->tm_year + 1900;
    month = t->tm_mon + 1;
    day = t->tm_mday;
    hour = t->tm_hour;
    min = t->tm_min;
    sec = t->tm_sec;
    log_d("%s %d-%d-%d-%d:%d:%d\n", year, month, day, hour, min, sec);
    return RET_OK;
}

u32 utils::read_uptime(void)
{
    u32 uptime;
    FILE *fp = fopen("/proc/uptime", "r");
    fscanf(fp, "%u", &uptime);
    if(fp != NULL) fclose(fp);
    log_d("%s uptime[%u]\n", __func__, uptime);
    return uptime;
}

int utils::read_cpu_usage(string &cpu)
{
    int load, total, percent;
    int user, n_user, sys, idle;
    FILE *fp;

    if((fp = fopen("/proc/stat", "r")) == NULL)
    {
        log_e("failed to open /proc/stat.\n");
        return RET_ERROR;
    }

    fscanf(fp, "%*s %d %d %d %d", &user, &n_user, &sys, &idle);
    fclose(fp);

    log_d("user[%d] nice_user[%d] system[%d] idle[%d]\n", 
                         user, n_user, sys, idle);

    // Find out the CPU load
    // user + sys = load
    // total = total
    load = user + n_user + sys;             // cpu.user + cpu.sys;
    total = user + n_user + sys + idle;     // cpu.total;

    if((load > 0) && (total > 0))
    {
        percent = (100 * load) / total;
    }

    cpu = to_string(percent) + "%";
    log_d("%s tot[%d] load[%d] usg[%s]\n", __func__, total, load, cpu.c_str());
    return RET_OK;
}

int utils::read_mem_usage(string &mem)
{
    int total, mfree, percent;
    FILE *fp;

    if((fp = fopen("/proc/meminfo", "r")) == NULL)
    {
        log_e("failed to open /proc/meminfo.\n");
        return RET_ERROR;
    }

    // cat /proc/meminfo
    // MemTotal:         125192 kB
    // MemFree:           39392 kB

    // total: free:
    fscanf(fp, "%*s %d %*s %*s %d", &total, &mfree);
    fclose(fp);

    // calculate it
    if(total != 0 && ((total - mfree) > 0))
        percent = (100 * (total - mfree)) / total;
    else
        percent = 0;

    mem = to_string(percent) + "%";
    log_d("%s tot[%d] free[%d] usg[%s]\n", __func__, total, mfree, mem.c_str());
    return percent;
}

int utils::read_version(string &ver)
{
#ifdef LINUX_PC_APP
    ver = "0.0.1";
#else
    FILE *fp;
    char buf[32] = {0,};

    if((fp = fopen("/etc/nap.version", "r")) == NULL)
    {
        log_e("failed to open /etc/nap.version \n");
        return RET_ERROR;
    }

    while(!feof(fp))
    {
        if(fgets(buf, sizeof(buf), fp) != nullptr)
        {
            ver.append(buf);
        }
    }

    fclose(fp);
    ver.erase(ver.find_last_not_of(" \n\r\t")+1);
#endif
    return RET_OK;
}

int utils::system_reboot(void)
{
    log_i("%s\n", __func__);
#ifdef LINUX_PC_APP
    // do nothing
#else
    usleep(100 * 1000); // 100 msec
    system("reboot");
#endif
    return RET_OK;
}

int utils::system_factory_reset(void)
{
    log_i("%s\n", __func__);
#ifdef LINUX_PC_APP
        // do nothing
#else
    usleep(100 * 1000); // 100 msec
    system("rm -rf /data/*");
    usleep(100 * 1000); // 100 msec
    system("reboot");
#endif
    return RET_OK;
}

int utils::system_call(string &cmd, string &result)
{
#ifdef LINUX_PC_APP
    log_d("%s\n", __func__);
#else
    FILE* fp = popen(cmd.c_str(), "r");
    if(fp != NULL)
    {
        while(!feof(fp))
        {
            char buf[256] = {0,};
            int len = fread(buf, 1, sizeof(buf) - 1, fp);
            if(!ferror(fp))
            {
                result.append(buf, len);
            }
        }
        pclose(fp);
    }

    log_d("%s\n%s\n%s\n", __func__, cmd.c_str(), result.c_str());
#endif
    return RET_OK;
}

int utils::system_call(string &cmd)
{
    int result = RET_OK;
    result = system(cmd.c_str());
    log_d("%s %s [%d]\n", __func__, cmd.c_str(), result);
    return result;
}

u32 utils::file_size(string file_path)
{
    u32 file_sz = 0;
    FILE *fp = fopen(file_path.c_str(), "r");
    if(fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        file_sz = ftell(fp);
        fclose(fp);
    }
    log_v("%s %s sz[%d]\n", __func__, file_path.c_str(), file_sz);
    return file_sz;
}

int utils::nvram_ontime_init(void)
{
    int result = RET_OK;
#ifdef LINUX_PC_APP
#else
    nvram_init();
#endif
    log_d("%s\n", __func__);
    return result;
}

int utils::nvram_uboot_get(string &key, string &value)
{
    int result = RET_OK;
#ifdef LINUX_PC_APP
#else
    //nvram_bufget(UBOOT_NVRAM, "ipaddr");
    char *ptr = nvram_bufget(UBOOT_NVRAM, (char*)key.c_str());
    int len = strlen(ptr);
    value = "";
    for(int i = 0; i < strlen(ptr); i++)
    {
        if(ptr[i] >= 0x20 && ptr[i] <= 0x7E)
        {
            value += ptr[i];
        }
    }
#endif
    log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    return result;
}

int utils::nvram_uboot_set(string &key, string &value)
{
    int result = RET_OK;
#ifdef LINUX_PC_APP
#else
    //nvram_bufset(UBOOT_NVRAM, "ipaddr", "192.168.0.100");
    nvram_bufset(UBOOT_NVRAM, (char*)key.c_str(), (char*)value.c_str());
#endif
    log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    return result;
}

int utils::nvram_rt2860_get(string &key, string &value)
{
    int result = RET_OK;
#ifdef LINUX_PC_APP
#else
    //nvram_bufget(RT2860_NVRAM, "NTPServer");
    char *ptr = nvram_bufget(RT2860_NVRAM, (char*)key.c_str());
    value = ptr;
#endif
    log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    return result;
}

int utils::nvram_rt2860_set(string &key, string &value)
{
    int result = RET_OK;
#ifdef LINUX_PC_APP
#else
    //nvram_bufset(RT2860_NVRAM, "NTPServer", "time.nist.gov");
    nvram_bufset(RT2860_NVRAM, (char*)key.c_str(), (char*)value.c_str());
#endif
    log_d("%s %s[%s]\n", __func__, key.c_str(), value.c_str());
    return result;
}

int utils::nvram_rt2860_commit(void)
{
    nvram_commit(1);
    return RET_OK;
}

int utils::modem_gps_updating_check(void)
{
    int ret_val = RET_ERROR;
    if (access("/tmp/GPSupdating", F_OK) == 0)
    {
        ret_val = RET_OK;
    }
    return ret_val;
}

int utils::modem_gps_get(double &longitude, double &latitude)
{
#ifdef LINUX_PC_APP
    return RET_ERROR;
#else
    int ret_val = RET_ERROR;
    FILE *fp = NULL;
    fp = fopen("/data/modem.conf", "r");
    if(fp != NULL)
    {
        cJSON *json_obj = NULL;
        char buf[4096] = {0,};
        if(fread(buf, 1, sizeof(buf), fp) > 0)
        {
            json_obj = cJSON_Parse((const char*)buf);
        }
        fclose(fp);

        if(json_obj != NULL)
        {
            cJSON *res = cJSON_GetObjectItemCaseSensitive(json_obj, "longitude");
            if(res != NULL)
            {
                longitude = res->valuedouble;
                ret_val = RET_OK;
            }

            res = cJSON_GetObjectItemCaseSensitive(json_obj, "latitude");
            if(res != NULL)
            {
                latitude = res->valuedouble;
                ret_val = RET_OK;
            }
        }
        cJSON_Delete(json_obj);
    }

     return ret_val;
#endif
}

int utils::modem_conf_get(string key, int &value)
{
    int ret_val = RET_ERROR;
#ifdef LINUX_PC_APP
    return ret_val;
#else
    FILE *fp = NULL;
    fp = fopen("/data/modem.conf", "r");
    if(fp != NULL)
    {
        cJSON *json_obj = NULL;
        char buf[4096] = {0,};
        if(fread(buf, 1, sizeof(buf), fp) > 0)
        {
            json_obj = cJSON_Parse((const char*)buf);
        }
        fclose(fp);

        if(json_obj != NULL)
        {
            cJSON *res = cJSON_GetObjectItemCaseSensitive(json_obj, key.c_str());
            if(res != NULL)
            {
                value = res->valueint;
                ret_val = RET_OK;
            }
        }
        cJSON_Delete(json_obj);
    }
#endif
    return ret_val;
}

int utils::modem_conf_get(string key, double &value)
{
    int ret_val = RET_ERROR;
#ifdef LINUX_PC_APP
    return ret_val;
#else
    FILE *fp = NULL;
    fp = fopen("/data/modem.conf", "r");
    if(fp != NULL)
    {
        cJSON *json_obj = NULL;
        char buf[4096] = {0,};
        if(fread(buf, 1, sizeof(buf), fp) > 0)
        {
            json_obj = cJSON_Parse((const char*)buf);
        }
        fclose(fp);

        if(json_obj != NULL)
        {
            cJSON *res = cJSON_GetObjectItemCaseSensitive(json_obj, key.c_str());
            if(res != NULL)
            {
                value = res->valuedouble;
                ret_val = RET_OK;
            }
        }
        cJSON_Delete(json_obj);
    }
#endif
    return ret_val;
}

int utils::modem_conf_get(string key, string &value)
{
    int ret_val = RET_ERROR;
#ifdef LINUX_PC_APP
    return ret_val;
#else
    FILE *fp = NULL;
    fp = fopen("/data/modem.conf", "r");
    if(fp != NULL)
    {
        cJSON *json_obj = NULL;
        char buf[4096] = {0,};
        if(fread(buf, 1, sizeof(buf), fp) > 0)
        {
            json_obj = cJSON_Parse((const char*)buf);
        }
        fclose(fp);

        if(json_obj != NULL)
        {
            cJSON *res = cJSON_GetObjectItemCaseSensitive(json_obj, key.c_str());
            if(res != NULL)
            {
                if(strlen(res->valuestring) > 0)
                {
                    value = res->valuestring;
                    ret_val = RET_OK;
                }
            }
        }
        cJSON_Delete(json_obj);
    }
#endif
    return ret_val;
}

int utils::hex_printf(char *data, u8 len)
{
    int ret_val = RET_OK;
    int i = 0;
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    string hex_str = "";
    for(i = 0; i < len; i++)
    {
        char const byte = data[i];
        hex_str += hex_chars[ ( byte & 0xF0 ) >> 4 ];
        hex_str += hex_chars[ ( byte & 0x0F ) >> 0 ];
    }

    debug_printf("%s\n", hex_str.c_str());
    return ret_val;
}

int utils::dump_log_file(string &str_dump)
{
    int ret_val = RET_OK;
    FILE* fp = NULL;
    char buf[256] = {0,};

    string file_name = "/data/nap-client.log.bak";
    u32 file_sz = utils::file_size(file_name);
    if(file_sz < (LOG_FILE_SIZE_MAX + (32*1024))) // 512 + 32 kbyte
    {
        fp = fopen(file_name.c_str(), "rt");
        if(fp != NULL)
        {
            while(!feof(fp))
            {
                if(fgets(buf, sizeof(buf), fp) != nullptr)
                {
                    str_dump.append(buf);
                }
            }
            fclose(fp);
        }
    }

    fp = fopen("/data/nap-client.log", "rt"); // NAP_LOG_FILE
    if(fp != NULL)
    {
        while(!feof(fp))
        {
            if(fgets(buf, sizeof(buf), fp) != nullptr)
            {
                str_dump.append(buf);
            }
        }
        fclose(fp);
    }
    //log_i("%s", str_dump.c_str());
    return ret_val;
}

int utils::udhcpc_renewal(void)
{
    // cat /tmp/run/udhcpc-eth1.pid
    // kill -16 3460      // USR1
    int pid = 0;
    FILE *fp = fopen("/var/run/udhcpc-eth1.pid", "r");
    if(fp)
    {
        fscanf(fp, "%d", &pid);
        fclose(fp);

        if(pid)
        {
            kill((pid_t)pid, SIGUSR1); // dhcp renew
        }
    }

    log_i("%s pid[%d]\n", __func__, pid);
    return RET_OK;
}

