#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

#include <string>
#include <list>

#include "log.h"
#include "utils.h"
#include "ep-manager.h"

#ifdef LINUX_PC_APP
  #define EPLOG_FILE       "ep-ul.log"
  #define EPLOG_FILE_BAK   "ep-ul.log.bak"
#else
  #define EPLOG_FILE       "/data/ep-ul.log"
  #define EPLOG_FILE_BAK   "/data/ep-ul.log.bak"
#endif

#define EP_LOG_ITEM_MAX    50

ep_manager::ep_manager() :
    _ep_item()
{
}

ep_manager::~ep_manager()
{
}

int ep_manager::init()
{
    int ret_val = RET_OK;
    log_d("ep_manager::%s \n", __func__);
    ep_log_read();
    return ret_val;
}

int ep_manager::deinit(void)
{
    log_d("%s\n", __func__);
    _ep_item.clear();
    return RET_OK;
}

int ep_manager::ep_log_read(void)
{
    int ret_val = RET_ERROR;
    FILE* fp = NULL;
    char buf[256] = {0,};

    _ep_item.clear();
    if(access(EPLOG_FILE, F_OK) == 0)
    {
        fp = fopen(EPLOG_FILE, "rt");
        log_i("%s %s\n", __func__, EPLOG_FILE);
    }
    else if(access(EPLOG_FILE_BAK, F_OK) == 0)
    {
        fp = fopen(EPLOG_FILE_BAK, "rt");
        log_i("%s %s\n", __func__, EPLOG_FILE_BAK);
    }

    if(fp != NULL)
    {
        while(!feof(fp))
        {
            if(fgets(buf, sizeof(buf), fp) != nullptr)
            {
                if(_ep_item.size() >= EP_LOG_ITEM_MAX)
                {
                    _ep_item.pop_front();
                }
                _ep_item.push_back(string(buf));
            }
        }
    }
    else
    {
        log_w("%s %s open fail !!!\n", __func__, EPLOG_FILE);
        goto free_all;
    }

    ret_val = RET_OK;

free_all:
    if(fp != NULL) fclose(fp);

    log_i("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ep_manager::ep_log_write(dat_grpc_frame::item_list &ul_dat)
{
    int ret_val = RET_ERROR;
    FILE* fp = NULL;
    char buf[256] ={0,};
    string str_time = "";
    list<string>::iterator iter;

    utils::convert_timestamp(ul_dat.timestamp, str_time);
    snprintf(buf, sizeof(buf),
        "rank[%d] oob[%d] id[%c%c%c%c%c%c%c%c] rssi[%d] freq[%.0f] timestamp[%s]\n",
            ul_dat.rank_ind,
            ul_dat.oob,
            ul_dat.data.at(10),
            ul_dat.data.at(11),
            ul_dat.data.at(8),
            ul_dat.data.at(9),
            ul_dat.data.at(6),
            ul_dat.data.at(7),
            ul_dat.data.at(4),
            ul_dat.data.at(5),
            ul_dat.rssi,
            ul_dat.freq,
            str_time.c_str());

    if(_ep_item.size() >= EP_LOG_ITEM_MAX)
    {
        _ep_item.pop_front();
    }
    _ep_item.push_back(string(buf));

    if(access(EPLOG_FILE_BAK, F_OK) == 0)
    {
        remove(EPLOG_FILE_BAK);
    }
    rename(EPLOG_FILE, EPLOG_FILE_BAK);

    fp = fopen(EPLOG_FILE, "wt");
    if (fp == NULL)
    {
        log_i("%s %s open fail !!!\n", __func__, EPLOG_FILE);
        goto free_all;
    }

    for(iter = _ep_item.begin(); iter != _ep_item.end(); ++iter)
    {
        fwrite(iter->c_str(), 1, iter->length(), fp);
    }

    ret_val = RET_OK;

free_all:
    if(fp != NULL) fclose(fp);

    log_d("%s ret_val[%d] \n", __func__, ret_val);
    return ret_val;
}

int ep_manager::ep_log_delete(void)
{
    log_i("%s \n", __func__);
    if(access(EPLOG_FILE, F_OK) == 0)
    {
        remove(EPLOG_FILE);
    }

    if(access(EPLOG_FILE_BAK, F_OK) == 0)
    {
        remove(EPLOG_FILE_BAK);
    }

    return RET_OK;
}

