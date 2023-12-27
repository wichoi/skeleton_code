#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/types.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/wireless.h>

#include <string>

#include <fw-api.h>

#include "log.h"
#include "config-manager.h"
#include "utils.h"

utils::utils()
{
}

utils::~utils()
{
}

u32 utils::read_ip_addr(string &ip_addr)
{
    int ret_val = RET_ERROR;
    struct ifreq ifr;
    char *ifname = "usb0";
    int skfd = 0;
    char if_addr[128] = {0,};

    if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        log_w("%s socket open error\n", __func__);
        return ret_val;
    }

    strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
    if(ioctl(skfd, SIOCGIFADDR, &ifr) < 0)
    {
        close(skfd);
        log_w("%s ioctl SIOCGIFADDR error for %s\n", __func__, ifname);
        return ret_val;
    }
    strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    close(skfd);

    ip_addr = if_addr;
    ret_val = RET_OK;
    log_v("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

u32 utils::read_link_state(bool &link_st)
{
    int ret_val = RET_OK;
    link_st = true; // usb0 always true
    log_v("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

u32 utils::read_interface_state(bool &if_st)
{
    int ret_val = RET_ERROR;
    struct ifreq ifr;
    char *ifname = "usb0";
    int skfd;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd == -1)
    {
        log_w("%s socket open error\n", __func__);
        return ret_val;
    }
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
    if(ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
    {
        log_w("%s ioctl error\n", __func__);
        close(skfd);
        return ret_val;
    }
    close(skfd);
    if (ifr.ifr_flags & IFF_UP)
    {
        if_st = true;
    }
    else
    {
        if_st = false;
    }

    ret_val = RET_OK;
    log_v("%s [%d]\n", __func__, ret_val);
    return ret_val;
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

u32 utils::read_random_time(string &start_tm, u32 span)
{
    // "startTm":"02:00:00", "span": 3600 (sec)

    u32 msec = 0;
    log_d("%s start_tm[%s] span[%d]", __func__, start_tm.c_str(), span);

    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
    char date_time[128] = { 0x0, };
    snprintf(date_time, sizeof(date_time), 
        "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);

    const char *delimiter = ":";
    char *ptr = NULL;
    char *next_ptr = NULL;
    int hour = 0;
    int min = 0;
    int sec = 0;

    // remove '"'
    start_tm.erase(std::remove(start_tm.begin(), start_tm.end(), '"'), start_tm.end());

    // hour
    ptr = strtok_r((char*)start_tm.c_str(), delimiter, &next_ptr);
    if(ptr != NULL) hour = (int)atoi(ptr);

    // min
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL) min = (int)atoi(ptr);

    // sec
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL) sec = (int)atoi(ptr);

    int rand_num = 0;
    if(span > 0)
    {
        srand((unsigned int)time(NULL));
        int num = rand();
        rand_num = num % span;
    }

    int cur_tm = (t->tm_hour * 3600) + (t->tm_min * 60) + t->tm_sec;
    int fix_tm = (hour * 3600) + (min * 60) + sec;
    if(fix_tm > cur_tm)
    {
        msec = fix_tm - cur_tm;
    }
    else
    {
        msec = (24 * 3600) - cur_tm + fix_tm;
    }

    // rand_num
    msec += rand_num;

    // convert to msec
    msec *= 1000;

    log_i("cur_tm[%s] start_tm[%d:%d:%d] rand_num[%d] timer_msec[%d]\n",
            date_time, hour, min, sec, rand_num, msec);
    return msec;
}

int utils::read_timestamp(string &timestamp)
{
    string cmd = "date -u \"+%Y-%m-%dT%H:%M:%SZ\"";
    utils::system_call(cmd, timestamp);
    timestamp.erase(std::remove(timestamp.begin(), timestamp.end(), '\n'),
                    timestamp.end());
#if 0
    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
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
#endif
    log_d("%s [%s]\n", __func__, timestamp.c_str());
    return RET_OK;
}

int utils::update_timestamp(void)
{
    int ret_val = RET_OK;
    time_t timer = time(NULL);
    struct tm *t = localtime(&timer);
    int local_year = t->tm_year + 1900;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    int modem_year = 0;
    utils::update_modem_cclk(year, month, day, hour, min, sec);
    modem_year = 2000 + year;
    log_i("local_time[%04d-%02d-%02d-%02d:%02d:%02d]\n",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
    log_i("modem_time[%04d-%02d-%02d-%02d:%02d:%02d]\n",
        year + 2000,
        month,
        day,
        hour,
        min,
        sec);

    if(year + 2000  != t->tm_year + 1900 ||
       month        != t->tm_mon + 1     ||
       day          != t->tm_mday        ||
       hour         != t->tm_hour        ||
       min          != t->tm_min         )
    {
        char date_time[128] = {0,};
        log_w("%s time mismatch set modem_time to local_time !!!\n", __func__);
        // "date -s \"2023-09-27 18:32:40\""
        snprintf(date_time, sizeof(date_time) - 1,
                "date -s \"20%02d-%02d-%02d %02d:%02d:%02d\"",
                year, month, day, hour, min, sec);
        string cmd = date_time;
        utils::system_call(cmd);
        ret_val = RET_ERROR;
    }

    return ret_val;
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

    char str_cpu[32] = {0,};
    sprintf(str_cpu, "%d%%", percent);
    cpu = str_cpu;
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

    char str_mem[32] = {0,};
    sprintf(str_mem, "%d%%", percent);
    mem = str_mem;
    log_d("%s tot[%d] free[%d] usg[%s]\n", __func__, total, mfree, mem.c_str());
    return percent;
}

int utils::system_reboot(void)
{
    log_i("%s\n", __func__);
    usleep(100 * 1000); // 100 msec
    system("reboot");
    return RET_OK;
}

int utils::system_call(string &cmd, string &result)
{
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
    return RET_OK;
}

int utils::system_call(string &cmd)
{
    int result = RET_OK;
    result = system(cmd.c_str());
    log_d("%s %s [%d]\n", __func__, cmd.c_str(), result);
    return result;
}

int utils::file_size(string &file_path)
{
    int file_sz = 0;
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

int utils::dump_log_file(string &str_dump)
{
    int ret_val = RET_OK;
    FILE* fp = NULL;
    char buf[256] = {0,};
#if 0
    fp = fopen("/tmp/p520-client.log.bak", "rt");
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
#endif
    fp = fopen("/tmp/520-client.log", "rt");
    if(fp != NULL)
    {
#if 0
        int file_sz = 0;
        int offset_sz = -(32 * 1024);
        fseek(fp, 0, SEEK_END);
        file_sz = ftell(fp);
        if(file_sz > offset_sz)
        {
            fseek(fp, offset_sz, SEEK_END);
        }
        else
        {
            fseek(fp, 0, SEEK_SET);
        }
#endif
        while(!feof(fp))
        {
            if(fgets(buf, sizeof(buf), fp) != NULL)
            {
                str_dump.append(buf);
            }
        }
        fclose(fp);
    }
    //log_i("%s", str_dump.c_str());
    return ret_val;
}

int utils::at_command(string &atcmd, string &result)
{
    int ret_val = RET_OK;
    string cmd = "riltest ATTEST:";
    cmd.append(atcmd);
    utils::system_call(cmd, result);
    log_d("%s cmd[%s] \n%s\n", __func__, atcmd.c_str(), result.c_str());
    return ret_val;
}

int utils::nvram_get_2860(string &key, string &value)
{
    int ret_val = RET_OK;
    string cmd = "nvram_get 2860 ";
    cmd.append(key);
    utils::system_call(cmd, value);
    value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());
    value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
    log_d("%s cmd[%s] value[%s]\n", __func__, cmd.c_str(), value.c_str());
    return ret_val;
}

int utils::nvram_set_2860(string &key, string &value)
{
    int ret_val = RET_OK;
    string cmd = "nvram_set 2860 ";
    cmd.append(key);
    cmd.append(" ");
    if(value.length() > 0)
    {
        cmd.append(value);
        cmd.append("");
    }
    else
    {
        cmd.append("\"\"");
    }
    utils::system_call(cmd);
    log_d("%s cmd[%s]\n", __func__, cmd.c_str());
    return ret_val;
}

int utils::http_header_parser(string &header)
{
    /*
    HTTP/1.1 200 
    Date: Wed, 05 Apr 2023 06:45:45 GMT
    Content-Type: application/json
    Transfer-Encoding: chunked
    Connection: keep-alive
    Vary: Origin
    Vary: Access-Control-Request-Method
    Vary: Access-Control-Request-Headers
    X-Content-Type-Options: nosniff
    X-XSS-Protection: 1; mode=block
    Cache-Control: no-cache, no-store, max-age=0, must-revalidate
    Pragma: no-cache
    Expires: 0
    Strict-Transport-Security: max-age=15724800; includeSubDomains
    X-Frame-Options: DENY
    */

    int ret_val = RET_OK;
    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";

    log_d("%s\n%s\n", __func__, header.c_str());
    ptr = strtok_r((char*)header.c_str(), delimiter, &next_ptr);
    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(str_value.find("Authorization:") != string::npos)
        {
            str_value.erase(0, strlen("Authorization:"));
            str_value.erase(std::remove(str_value.begin(), str_value.end(), ' '), str_value.end());
            str_value.erase(std::remove(str_value.begin(), str_value.end(), '"'), str_value.end());
            str_value.erase(std::remove(str_value.begin(), str_value.end(), '\r'), str_value.end());
            str_value.erase(std::remove(str_value.begin(), str_value.end(), '\n'), str_value.end());
            log_i("Authorization[%s]\n", str_value.c_str());
            config_manager::instance()->set_cloud_token(str_value);
            break;
        }
        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::get_modem_sms(string &sms_body, int index)
{
    /*
    AT$$CMGR=0
    $$CMGR: 20230413112514,01229991791,"01030150165",test1234
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "ATTEST:AT$\$CMGR=";
    char at_resp[128] = {0,};
    char unsol_resp[128] = {0,};
    char tmp[8] = {0,};
    snprintf(tmp, sizeof(tmp) - 1, "%d", index);
    atcmd.append(tmp);
    sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 0);
    sms_body = at_resp;
    log_i("%s ix[%d] data[%s]\n", __func__, index, sms_body.c_str());
    return ret_val;
}

int utils::rm_modem_sms(void)
{
    /*
    AT+CMGD=,4
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "ATTEST:AT+CMGD=,4";
    char at_resp[128] = {0,};
    char unsol_resp[128] = {0,};
    sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 0);
    log_i("%s\n", __func__);
    return ret_val;
}

int utils::modem_sms_command(int &mdm_reset, int &mdm_alive)
{
    // *25987309*100*   - ImAlive Event
    // *147359*330*     - device reboot
    // *147359*331*     - device reboot

    int ret_val = RET_OK;
    string sms_body = "";
    int i = 0;
    // SMS index : 0 ~ 19
    for(i = 0; i < 20; i++)
    {
        sms_body.clear();
        get_modem_sms(sms_body, i);
        if(strstr(sms_body.c_str(), "*25987309*100*") != NULL)
        {
            mdm_alive = 1;
        }
        else if(strstr(sms_body.c_str(), "*147359*330*") != NULL)
        {
            mdm_reset = 1;
        }
        else if(strstr(sms_body.c_str(), "*147359*331*") != NULL)
        {
            mdm_reset = 1;
        }
    }
    rm_modem_sms();

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int utils::set_modem_band(string &band)
{
    int ret_val = RET_ERROR;
    string cur_band = config_manager::instance()->get_modem_band_change();
    if(cur_band.compare(band) !=  0)
    {
        band.erase(std::remove(band.begin(), band.end(), ','), band.end());
        if(band.length() > 0)
        {
            // AT$$BANDCHANGE=831
            string atcmd = "ATTEST:AT$\$BANDCHANGE=";
            atcmd.append(band);
            char at_resp[128] = {0,};
            char unsol_resp[128] = {0,};
            sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 0);
            log_i("%s [%s][%s]\n", __func__, atcmd.c_str(), at_resp);
            ret_val = RET_OK;
        }
        else
        {
            log_i("%s invalid input !!!\n", __func__);
        }
    }
    else
    {
        log_i("%s bandchange already applied.\n", __func__);
    }

    return ret_val;
}

int utils::set_modem_apn(string &apn)
{
    int ret_val = RET_ERROR;

    if(apn.length() > 0)
    {
        string cur_apn = config_manager::instance()->get_modem_apn();
        if(cur_apn.compare(apn) !=  0)
        {
            // AT+CGDCONT=1,"IPV4V6","privatelte.ktfwing.com"
            string atcmd = "riltest ATTEST:AT+CGDCONT=1,\\\"IPV4V6\\\",\\\"";
            atcmd.append(apn);
            atcmd.append("\\\"");
            string result;
            utils::system_call(atcmd, result);
            log_i("%s [%s][%s]\n", __func__, atcmd.c_str(), result.c_str());
            ret_val = RET_OK;
        }
        else
        {
            log_i("%s apn already applied.\n", __func__);
        }
    }
    else
    {
        log_i("%s invalid input !!!\n", __func__);
    }

    return ret_val;
}

int utils::update_modem_ati(void)
{
    /*
    ATI
    Manufacturer: APROTECH
    Model: ATM-L710
    Revision: ATM-L710_V10.0.13  1  [May 26 2020 13:35:40]
    IMEI: 352580099942422
    +GCAP: +CGSM
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "ATI";
    string result = "";
    utils::at_command(atcmd, result);

    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";
    string::size_type pos = 0;

    log_d("%s\n%s\n", __func__, result.c_str());
    ptr = strtok_r((char*)result.c_str(), delimiter, &next_ptr);

    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(pos = str_value.find("Revision: ") != string::npos)
        {
            str_value.erase(0, strlen("Revision: "));
            str_value = str_value.substr(0, str_value.find(" "));
            log_d("mdm_ver[%s]\n", str_value.c_str());
            config_manager::instance()->set_modem_version(str_value);
        }
        else if(pos = str_value.find("IMEI: ") != string::npos)
        {
            str_value.erase(0, strlen("IMEI: "));
            str_value = str_value.substr(0, str_value.find(" "));
            log_d("imei[%s]\n", str_value.c_str());
            config_manager::instance()->set_modem_imei(str_value);
        }

        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::update_modem_cnum(void)
{
    /*
    AT+CNUM
    +CNUM: "Hello world","+821229991791",145
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "AT+CNUM";
    string result = "";
    utils::at_command(atcmd, result);

    const char *delimiter = ",";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";

    log_d("%s\n%s\n", __func__, result.c_str());

    // +CNUM: "Hello world"
    ptr = strtok_r((char*)result.c_str(), delimiter, &next_ptr);

    // "+821229991791"
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        str_value = ptr;
        str_value.erase(std::remove(str_value.begin(), str_value.end(), '"'), str_value.end());
        log_d("number[%s]\n", str_value.c_str());
        config_manager::instance()->set_modem_number(str_value);
    }

    return ret_val;
}

int utils::update_modem_serial(void)
{
    /*
    AT$$SERIALNUM?
    $$SERIALNUM:994242
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "ATTEST:AT$\$SERIALNUM?";
    char at_resp[128] = {0,};
    char unsol_resp[128] = {0,};
    sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 0);

    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";

    log_d("%s\n%s\n", __func__, at_resp);
    ptr = strtok_r(at_resp, delimiter, &next_ptr);
    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(str_value.find("$$SERIALNUM:") != string::npos)
        {
            str_value.erase(0, strlen("$$SERIALNUM:"));
            log_d("serial[%s]\n", str_value.c_str());
            config_manager::instance()->set_router_serial(str_value);
            break;
        }
        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::update_modem_sysinfo(void)
{
    /*
    AT+SYSINFO
    +SYSINFO: 2,9,0,1,1,1,450,08,3,1694,54,0,0,0,-78,-8,0,65,5,11588894,3,0,0,0,1,1,0,0,115,210
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "AT+SYSINFO";
    string result = "";
    utils::at_command(atcmd, result);

    const char *delimiter = ",";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";

    log_d("%s\n%s\n", __func__, result.c_str());

    // <srv_status> - +SYSINFO: 2
    ptr = strtok_r((char*)result.c_str(), delimiter, &next_ptr);

    // <sys_mode>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <roam_status>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <sim_state>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <voice_domain>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <sms_domain>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <mcc>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <mnc>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <band>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        log_d("band[%s]\n", ptr);
        config_manager::instance()->set_modem_band((int)atoi(ptr));
    }

    // <channel>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL) 
    {
        log_d("channel[%s]\n", ptr);
        config_manager::instance()->set_modem_frequency((int)atoi(ptr));
    }

    // <rssi>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        log_d("rssi[%s]\n", ptr);
        config_manager::instance()->set_modem_rssi((int)atoi(ptr));
    }

    // <rscp>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <bit_err_rate>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <ecio>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <rsrp>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        log_d("rsrp[%s]\n", ptr);
        config_manager::instance()->set_modem_rsrp((int)atoi(ptr));
    }

    // <rsrq>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        log_d("rsrq[%s]\n", ptr);
        config_manager::instance()->set_modem_rsrq((int)atoi(ptr));
    }

    // <lac>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <lte_tac>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <lte_rac>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <cell_id>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        char buf[128] ={0,};
        snprintf(buf, sizeof(buf) -1, "%X", atoi(ptr));
        str_value = buf;
        log_d("cell_id[%s]\n", str_value.c_str());
        config_manager::instance()->set_modem_cell_id(str_value);
    }

    // <emm_state>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <emm_substate>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <emm_c onnection_state>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <last_reg_cause>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <TIN>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <rrc_active>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <operator_call_barred>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <ui_ds_status>
    ptr = strtok_r(NULL, delimiter, &next_ptr);

    // <pci>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        log_d("pci[%s]\n", ptr);
        config_manager::instance()->set_modem_pci((int)atoi(ptr));
    }

    // <sinr>
    ptr = strtok_r(NULL, delimiter, &next_ptr);
    if(ptr != NULL)
    {
        int raw = (int)atoi(ptr); // 0 ~ 250
        int sinr = 99; // -20 ~ +30 
        if(raw >= 0 && raw <= 250)
        {
            sinr = raw / 5 - 20;
        }
        log_d("raw[%d] sinr[%d]\n", raw, sinr);
        config_manager::instance()->set_modem_sinr(sinr);
    }

    return ret_val;
}

int utils::update_modem_cgpaddr(void)
{
    /*
    AT+CGPADDR
    +CGPADDR: 1,10.222.103.30
    +CGPADDR: 2,0.0.0.0,0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "AT+CGPADDR";
    string result = "";
    utils::at_command(atcmd, result);

    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";
    string::size_type pos = 0;

    log_d("%s\n%s\n", __func__, result.c_str());
    ptr = strtok_r((char*)result.c_str(), delimiter, &next_ptr);

    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(pos = str_value.find("+CGPADDR: 1,") != string::npos)
        {
            str_value.erase(0, strlen("+CGPADDR: 1,"));
            log_d("cgpaddr[%s]\n", str_value.c_str());
            config_manager::instance()->set_modem_ip_addr(str_value);
            break;
        }

        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::update_modem_cgdcont(void)
{
    /*
    AT+CGDCONT?
    +CGDCONT: 1,"IPV4V6","privatelte.ktfwing.com","0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0",0,0,0,0
    +CGDCONT: 2,"IPV4V6","lte.ktfwing.com","0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0",0,0,0,0
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "AT+CGDCONT?";
    string result = "";
    utils::at_command(atcmd, result);

    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";
    string::size_type pos = 0;

    log_d("%s\n%s\n", __func__, result.c_str());
    ptr = strtok_r((char*)result.c_str(), delimiter, &next_ptr);

    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(pos = str_value.find("+CGDCONT: 1,") != string::npos)
        {
            str_value.erase(0, strlen("+CGDCONT: 1,"));

            delimiter = ",";
            // "IPV4V6"
            ptr = strtok_r((char*)str_value.c_str(), delimiter, &next_ptr);

            // "privatelte.ktfwing.com"
            ptr = strtok_r(NULL, delimiter, &next_ptr);
            if(ptr != NULL)
            {
                str_value = ptr;
                str_value.erase(std::remove(str_value.begin(), str_value.end(), '"'), str_value.end());
                log_d("apn[%s]\n", str_value.c_str());
                config_manager::instance()->set_modem_apn(str_value);
            }
            break;
        }

        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::update_modem_bandchange(void)
{
    /*
    AT$$BANDCHANGE?
    $$BAND:138,831
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "ATTEST:AT$\$BANDCHANGE?";
    char at_resp[128] = {0,};
    char unsol_resp[128] = {0,};
    sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 0);

    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";

    log_d("%s\n%s\n", __func__, at_resp);
    ptr = strtok_r(at_resp, delimiter, &next_ptr);
    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(str_value.find("$$BAND:") != string::npos)
        {
            str_value.erase(0, strlen("$$BAND:"));

            delimiter = ",";
            // 138
            ptr = strtok_r((char*)str_value.c_str(), delimiter, &next_ptr);
            // 831
            ptr = strtok_r(NULL, delimiter, &next_ptr);
            if(strlen(ptr)> 0)
            {
                str_value = ptr;
            }

            int i = 0;
            string temp_str = "";
            for(i = 0; i < str_value.length(); i++)
            {
                if(i != 0)
                {
                    temp_str.append(",");
                }
                temp_str.append(str_value, i, 1);
            }
            str_value = temp_str;

            log_d("%s [%s]\n", __func__, str_value.c_str());
            config_manager::instance()->set_modem_band_change(str_value);
            break;
        }
        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::update_modem_swver(void)
{
    /*
    AT$$SWVER?
    $$SWVER: ATML710V009R203M2G2G,Jan 13 2023,15:39:59
    */

    int ret_val = RET_OK;
    string atcmd = "ATTEST:AT$\$SWVER?";
    char at_resp[128] = {0,};
    char unsol_resp[128] = {0,};
    sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 0);

    const char *delimiter = "\n";
    char *ptr = NULL;
    char *next_ptr = NULL;
    string str_value = "";

    log_d("%s\n%s\n", __func__, at_resp);
    ptr = strtok_r(at_resp, delimiter, &next_ptr);
    while(ptr)
    {
        log_v("%s\n", ptr);
        str_value = ptr;
        if(str_value.find("$$SWVER: ") != string::npos)
        {
            str_value.erase(0, strlen("$$SWVER: "));

            delimiter = ",";
            // ATML710V009R203M2G2G
            ptr = strtok_r((char*)str_value.c_str(), delimiter, &next_ptr);
            if(strlen(ptr)> 0)
            {
                str_value = ptr;
                log_d("%s [%s]\n", __func__, str_value.c_str());
                config_manager::instance()->set_modem_ver_ex(str_value);
            }
            break;
        }
        ptr = strtok_r(NULL, delimiter, &next_ptr);
    }

    return ret_val;
}

int utils::update_modem_cclk(int &y, int &m, int &d, int &h, int &min, int &s)
{
    /*
    AT+CCLK?
    +CCLK: "23/09/27,13:06:34+36"
    OK
    */

    int ret_val = RET_OK;
    string atcmd = "AT+CCLK?";
    string result = "";
    char *ptr = NULL;
    char *next_ptr = NULL;
    char date_time[128] = { 0x0, };

    utils::at_command(atcmd, result);

    log_d("%s\n%s\n", __func__, result.c_str());

    // +CCLK: "
    ptr = strtok_r((char*)result.c_str(), "\"", &next_ptr);

    // year
    ptr = strtok_r(NULL, "/", &next_ptr);
    if(ptr != NULL)
    {
        y = (int)atoi(ptr);
        log_d("year[%d]\n", y);
    }

    // month
    ptr = strtok_r(NULL, "/", &next_ptr);
    if(ptr != NULL)
    {
        m = (int)atoi(ptr);
        log_d("month[%d]\n", m);
    }

    // d
    ptr = strtok_r(NULL, ",", &next_ptr);
    if(ptr != NULL)
    {
        d = (int)atoi(ptr);
        log_d("day[%d]\n", d);
    }

    // hour
    ptr = strtok_r(NULL, ":", &next_ptr);
    if(ptr != NULL)
    {
        h = (int)atoi(ptr);
        log_d("hour[%d]\n", h);
    }

    // min
    ptr = strtok_r(NULL, ":", &next_ptr);
    if(ptr != NULL)
    {
        min = (int)atoi(ptr);
        log_d("minutes[%d]\n", min);
    }

    // sec
    ptr = strtok_r(NULL, "+", &next_ptr);
    if(ptr != NULL)
    {
        s = (int)atoi(ptr);
        log_d("seconds[%d]\n", s);
    }

    // "2023-09-27T04:45:32Z"
    log_d("%s 20%02d-%02d-%02dT%02d:%02d:%02dZ\n", __func__, y, m, d, h, min, s);
    return ret_val;
}

int utils::update_modem_info(void)
{
    int ret_val = RET_OK;

    utils::update_modem_ati();
    utils::update_modem_cnum();
    utils::update_modem_serial();
    utils::update_modem_sysinfo();
    utils::update_modem_cgpaddr();
    utils::update_modem_cgdcont();
    utils::update_modem_bandchange();
    utils::update_modem_swver();

    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

int utils::update_router_version(void)
{
    /*
    cat version 
    ATM-P520-KT-VER4.21[20190604-1621]
    ATM-P520
    KT
    VER4.21
    VER2.0
    ATM-P520-KT-4.40[20230404-1549]
    ATM_L710_ATML710V009R203M2G2G_ATML710V009R201M2G2G.zip
    V009R203-V009R201-2G
    ATM_L710_ATML710V009R201M4G2G_ATML710V009R202M4G2G.zip
    V009R201-V009R202-4G
    */

    int ret_val = RET_ERROR;
    FILE *fp = NULL;
    log_d("%s\n", __func__);
    fp = fopen("/etc_ro/version", "r");
    if(fp)
    {
        char buf[128] = {0,};
        string router_ver = "";
        string hidden_ver = "";

        fgets(buf, sizeof(buf), fp); // router official version
        router_ver = buf; // ATM-P520-KT-VER4.21[20190604-1621]
        fgets(buf, sizeof(buf), fp); // router model name
        fgets(buf, sizeof(buf), fp); // router service provider
        fgets(buf, sizeof(buf), fp); // router version
        fgets(buf, sizeof(buf), fp); // router hw version
        fgets(buf, sizeof(buf), fp); // router hidden version
        hidden_ver = buf; // ATM-P520-KT-4.40[20230404-1549]
        hidden_ver = hidden_ver.substr(0, hidden_ver.find("["));
        fgets(buf, sizeof(buf), fp); // modem full version
        fgets(buf, sizeof(buf), fp); // modem short version

        router_ver.erase(std::remove(router_ver.begin(), router_ver.end(), '\r'), router_ver.end());
        router_ver.erase(std::remove(router_ver.begin(), router_ver.end(), '\n'), router_ver.end());
        hidden_ver.erase(std::remove(hidden_ver.begin(), hidden_ver.end(), '\r'), hidden_ver.end());
        hidden_ver.erase(std::remove(hidden_ver.begin(), hidden_ver.end(), '\n'), hidden_ver.end());

        log_d("%s version[%s]\n", __func__, router_ver.c_str());
        log_d("%s ver_ex [%s]\n", __func__, hidden_ver.c_str());

        config_manager::instance()->set_router_version(router_ver);
        config_manager::instance()->set_router_ver_ex(hidden_ver);

        fclose(fp);
        ret_val = RET_OK;
    }

    return ret_val;
}

int utils::update_router_usb0_tx(void)
{
    int ret_val = RET_ERROR;
    u32 data = 0;

    // tx_bytes, rx_bytes, tx_errors, rx_errors
    // tx_packets, rx_packets,  rx_errors, tx_errors, tx_bytes ...
    string path = "/sys/class/net/usb0/statistics/tx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
        config_manager::instance()->set_router_tx(data);
        ret_val = RET_OK;
    }

    log_d("result[%d] usb0 tx[%d]\n", ret_val, data);
    return ret_val;
}

int utils::update_router_usb0_rx(void)
{
    int ret_val = RET_ERROR;
    u32 data = 0;

    // tx_bytes, rx_bytes, tx_errors, rx_errors
    // tx_packets, rx_packets,  rx_errors, tx_errors, tx_bytes ...
    string path = "/sys/class/net/usb0/statistics/rx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%u", &data);
        fclose(fp);
        config_manager::instance()->set_router_rx(data);
        ret_val = RET_OK;
    }

    log_d("result[%d] usb0 tx[%d]\n", ret_val, data);
    return ret_val;
}

int utils::update_router_eth2_tx(void)
{
    int ret_val = RET_ERROR;
    u32 data = 0;

    // tx_bytes, rx_bytes, tx_errors, rx_errors
    // tx_packets, rx_packets,  rx_errors, tx_errors, tx_bytes ...
    string path = "/sys/class/net/eth2/statistics/tx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%d", &data);
        fclose(fp);
        ret_val = RET_OK;
    }

    log_d("result[%d] usb0 tx[%d]\n", ret_val, data);
    return ret_val;
}

int utils::update_router_eth2_rx(void)
{
    int ret_val = RET_ERROR;
    u32 data = 0;

    // tx_bytes, rx_bytes, tx_errors, rx_errors
    // tx_packets, rx_packets,  rx_errors, tx_errors, tx_bytes ...
    string path = "/sys/class/net/eth2/statistics/rx_bytes";
    FILE *fp = fopen(path.c_str(), "r");
    if(fp != NULL)
    {
        fscanf(fp, "%d", &data);
        fclose(fp);
        ret_val = RET_OK;
    }

    log_d("result[%d] usb0 tx[%d]\n", ret_val, data);
    return ret_val;
}

int utils::update_router_info(void)
{
    int ret_val = RET_OK;

    char buf[128] = {0,};
    snprintf(buf, sizeof(buf) -1, "%d", read_uptime());
    string up_time = buf;
    config_manager::instance()->set_router_uptime(up_time);

    utils::update_router_version();
    utils::update_router_usb0_tx();
    utils::update_router_usb0_rx();

    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

static char b64_encode(unsigned char u)
{
    if (u < 26) return 'A' + u;
    if (u < 52) return 'a' + (u - 26);
    if (u < 62) return '0' + (u - 52);
    if (u == 62) return '+';
    return '/';
}

static unsigned char b64_decode(char c)
{
    if (c >= 'A' && c <= 'Z') return (c - 'A');
    if (c >= 'a' && c <= 'z') return (c - 'a' + 26);
    if (c >= '0' && c <= '9') return (c - '0' + 52);
    if (c == '+') return 62;
    return 63;
}

static int is_base64(char c)
{
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') || (c == '+') || (c == '/') || (c == '=')) {
        return 1;
    }

    return 0;
}

int utils::encode_base64(string& dest, const string& src)
{
    int ret_val = RET_ERROR;
    if (src.length() == 0) return ret_val;

    int size = src.length();
    dest.clear();

    for (int i = 0; i < size; i += 3)
    {
        unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0;
        b1 = src[i];

        if (i + 1 < size) b2 = src[i + 1];
        if (i + 2 < size) b3 = src[i + 2];

        b4 = b1 >> 2;
        b5 = ((b1 & 0x3) << 4) | (b2 >> 4);
        b6 = ((b2 & 0xf) << 2) | (b3 >> 6);
        b7 = b3 & 0x3f;

        dest += b64_encode(b4);
        dest += b64_encode(b5);

        if (i + 1 < size)
            dest += b64_encode(b6);
        else
            dest += '=';

        if (i + 2 < size)
            dest += b64_encode(b7);
        else
            dest += '=';
    }

    ret_val = RET_OK;
    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

int utils::decode_base64(string& dest, const string& src)
{
    int ret_val = RET_ERROR;
    if (src.length() == 0) return false;

    int size = src.length();
    dest.clear();

    for (int k = 0; k < size; k += 4)
    {
        char c1 = 'A', c2 = 'A', c3 = 'A', c4 = 'A';
        unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0;
        c1 = src[k];

        if (k + 1 < size) c2 = src[k + 1];
        if (k + 2 < size) c3 = src[k + 2];
        if (k + 3 < size) c4 = src[k + 3];

        if (!is_base64(c1) || !is_base64(c1) || !is_base64(c1) || !is_base64(c1))
            return false;

        b1 = b64_decode(c1);
        b2 = b64_decode(c2);
        b3 = b64_decode(c3);
        b4 = b64_decode(c4);

        dest += ((b1 << 2) | (b2 >> 4));

        if (c3 != '=') dest += (((b2 & 0xf) << 4) | (b3 >> 2));
        if (c4 != '=') dest += (((b3 & 0x3) << 6) | b4);
    }

    ret_val = RET_OK;
    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

int utils::encode_base64(string& dest, const unsigned char *src, int size)
{
    int ret_val = RET_ERROR;
    dest.clear();

    for (int i = 0; i < size; i += 3)
    {
        unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0;
        b1 = src[i];

        if (i + 1 < size) b2 = src[i + 1];
        if (i + 2 < size) b3 = src[i + 2];

        b4 = b1 >> 2;
        b5 = ((b1 & 0x3) << 4) | (b2 >> 4);
        b6 = ((b2 & 0xf) << 2) | (b3 >> 6);
        b7 = b3 & 0x3f;

        dest += b64_encode(b4);
        dest += b64_encode(b5);

        if (i + 1 < size)
            dest += b64_encode(b6);
        else
            dest += '=';

        if (i + 2 < size)
            dest += b64_encode(b7);
        else
            dest += '=';
    }

    ret_val = RET_OK;
    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

int utils::decode_base64(char *dest, int *len, const string& src)
{
    int ret_val = RET_ERROR;
    if (src.length() == 0) return false;
    *len = 0;

    int size = src.length();
    for (int k = 0; k < size; k += 4)
    {
        char c1 = 'A', c2 = 'A', c3 = 'A', c4 = 'A';
        unsigned char b1 = 0, b2 = 0, b3 = 0, b4 = 0;
        c1 = src[k];

        if (k + 1 < size) c2 = src[k + 1];
        if (k + 2 < size) c3 = src[k + 2];
        if (k + 3 < size) c4 = src[k + 3];

        if (!is_base64(c1) || !is_base64(c1) || !is_base64(c1) || !is_base64(c1))
            return false;

        b1 = b64_decode(c1);
        b2 = b64_decode(c2);
        b3 = b64_decode(c3);
        b4 = b64_decode(c4);

        dest[(*len)++] = ((b1 << 2) | (b2 >> 4));

        if (c3 != '=') dest[(*len)++] = (((b2 & 0xf) << 4) | (b3 >> 2));
        if (c4 != '=') dest[(*len)++] = (((b3 & 0x3) << 6) | b4);
    }

    ret_val = RET_OK;
    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

bool utils::hex_to_char(char const* szHex, unsigned char* rch)
{
    char ch = toupper(*szHex);
    if(ch >= '0' && ch <= '9')
        *rch = ch - '0';
    else if(ch >= 'A' && ch <= 'F')
        *rch = ch - 55; //-'A' + 10
    else
        //Is not really a Hex string
        return false; 

    szHex++;
    ch = toupper(*szHex);
    if(ch >= '0' && ch <= '9')
        (*rch <<= 4) += ch - '0';
    else if(ch >= 'A' && ch <= 'F')
        (*rch <<= 4) += ch - 55; //-'A' + 10;
    else
        //Is not really a Hex string
        return false;

 return true;
}

int utils::hex_to_string(string &dest, string &src)
{
    int ret_val = RET_OK;
    unsigned char dest_ch;
    int i = 0;
    dest.clear();
    for(i = 0; i < src.length(); i+=2)
    {
        utils::hex_to_char((const char*)&src[i], &dest_ch);
        dest += dest_ch;
    }

    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

int utils::cloud_hostbyname(string &url, list<string> &ip_list)
{
    int ret_val = RET_OK;
    //string url = config_manager::instance()->get_cloud_url();
    //url.erase(0, strlen("http://"));
    //url.erase(0, strlen("https://"));
    struct hostent *host;
    int i = 0;
    ip_list.clear();
    host = gethostbyname(url.c_str());
    log_i("%s\n", host->h_name);
    for(i = 0; host->h_aliases[i] != NULL; i++)
    {
        log_i("aliases[%s]\n", host->h_aliases[i]);
    }
 
    for(i = 0; host->h_addr_list[i] != NULL; i++)
    {
        ip_list.push_back(inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
        log_i("addrList[%s]\n", inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
    }
 
    log_d("%s [%d]\n", __func__, ret_val);
    return ret_val;
}

