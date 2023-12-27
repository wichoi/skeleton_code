#ifndef __JSON_MANAGER_H__
#define __JSON_MANAGER_H__

#include <string>
#include <list>

#include "common.h"

// auth
#define JSON_KEY_MODEL          "model"
#define JSON_KEY_IMEI           "imei"
#define JSON_KEY_SERIAL_NUM     "sn"

// sauth
#define JSON_KEY_SINFO          "sinfo"
#define JSON_KEY_KHINT          "khint"

// boot
#define JSON_KEY_MDM_VER        "msw"
#define JSON_KEY_DEV_VER        "dsw"
#define JSON_KEY_SCTN           "sctn"
#define JSON_KEY_TIMESTAMP      "ts"
#define JSON_KEY_REASON         "reason"
#define JSON_KEY_PARAMS         "params"

// event
#define JSON_KEY_NM             "nm"
#define JSON_KEY_STS            "sts"
#define JSON_KEY_ETS            "ets"
#define JSON_KEY_REASON         "reason"
#define JSON_KEY_IP             "ip"
#define JSON_KEY_TS             "ts"
#define JSON_KEY_PORT           "port"
#define JSON_KEY_PN             "pn"
#define JSON_KEY_PS             "ps"

// report (modem)
#define JSON_KEY_MODEM_OBJ      "mr"
#define JSON_KEY_CELL_ID_NUM    "pci"
#define JSON_KEY_CELL_ID_STR    "cellId"
#define JSON_KEY_FREQUENCY      "freq"
#define JSON_KEY_BAND           "band"
#define JSON_KEY_IP_ADDR        "ip"
#define JSON_KEY_RSSI           "rssi"
#define JSON_KEY_RSRP           "rsrp"
#define JSON_KEY_RSRQ           "rsrq"
#define JSON_KEY_SINR           "sinr"

// report (router)
#define JSON_KEY_ROUTER_OBJ     "dr"
#define JSON_KEY_TX             "tx"
#define JSON_KEY_RX             "rx"
#define JSON_KEY_UPTIME         "uptime"
#define JSON_KEY_RESET_CNT      "mRstCnt"
#define JSON_KEY_IP_CHANGE_CNT  "ipChgCnt"
#define JSON_KEY_ERROR_CNT      "netErrCnt"
#define JSON_KEY_LAN_PORT       "lanPort"
#define JSON_KEY_MEM_USAGE      "umem"

// paramupdated
#define JSON_KEY_PTS            "pts"

// getfwup
#define JSON_KEY_TP             "tp"
#define JSON_KEY_VERSION        "ver"

// fwup
#define JSON_KEY_STATUS         "status"
#define JSON_KEY_FAIL_REASON    "failReason"
#define JSON_KEY_ERR            "err"

// response
#define JSON_KEY_RES_RET        "ret"
#define JSON_KEY_RES_TS         "ts"
#define JSON_KEY_RES_ERR        "err"
#define JSON_KEY_RES_RETRY      "retryTM"
#define JSON_KEY_RES_CMDS       "cmds"
#define JSON_KEY_RES_RPT_TIME   "repTm"
#define JSON_KEY_RES_HB_TIME    "hbTm"
#define JSON_KEY_RES_CS         "cs"
#define JSON_KEY_RES_SKEY       "skey"
#define JSON_KEY_RES_PARAMS     "params"
#define JSON_KEY_RES_TP         "tp"
#define JSON_KEY_RES_UP_VER     "upVer"
#define JSON_KEY_RES_URL        "url"
#define JSON_KEY_RES_CS_URL     "csUrl"
#define JSON_KEY_RES_START_TM   "startTm"
#define JSON_KEY_RES_SPAN_TM    "span"

// object key
#define JSON_OBJ_KEY_NM         "nm"
#define JSON_OBJ_KEY_CMD        "cmd"
#define JSON_OBJ_KEY_TP         "tp"
#define JSON_OBJ_KEY_SCMD       "scmd"
#define JSON_OBJ_KEY_DESC       "desc"

// object value
#define JSON_OBJ_UPDATE_KEY     "UpdateKey"
#define JSON_OBJ_UPDATE_AUTH    "UpdateAuth"
#define JSON_OBJ_UPDATE_PARAM   "UpdateParam"
#define JSON_OBJ_RESET          "Reset"
#define JSON_OBJ_SYS_CMD        "SysCmd"
#define JSON_OBJ_UPDATE_FW      "UpdateFw"

// object param
#define JSON_PARAM_PTS          "pts"
#define JSON_PARAM_RPT_INTV     "repIntv"
#define JSON_PARAM_HB_INTV      "hbIntv"
#define JSON_PARAM_EV_FLAGS     "evFlags"
#define JSON_PARAM_BAND         "band"
#define JSON_PARAM_APN          "apn"

// object error
#define JSON_OBJ_ERR_CODE       "code"
#define JSON_OBJ_ERR_DESC       "desc"

// object event
#define JSON_OBJ_NER_ERR        "NetworkError"
#define JSON_OBJ_IP_CHANGE      "WanIpChanged"
#define JSON_OBJ_LAN_CHANGE     "LanPortChanged"
#define JSON_OBJ_SMS_ALIVE      "ImAlive"

// return value
#define JSON_RET_OK             "OK"
#define JSON_RET_ERROR          "Error"
#define JSON_RET_UNAUTH         "Unauthorized"
#define JSON_RET_EXPIRED        "Expired"
#define JSON_RET_NOT_FOUNT      "NotFound"

// fwup status
#define JSON_FW_ST_DOWNLOADING  "Downloading"
#define JSON_FW_ST_DOWNLOADED   "Downloaded"
#define JSON_FW_ST_INSTALLING   "Installing"
#define JSON_FW_ST_INSTALLED    "Installed"
#define JSON_FW_ST_FAILED       "InstallFailed"

// fwup reason
#define JSON_FW_ERR_INTERNAL    "InternalError"     // 1
#define JSON_FW_ERR_NET         "NetAccess"         // 2
#define JSON_FW_ERR_FILE        "FileAccess"        // 3
#define JSON_FW_ERR_URL         "InvalidUrl"        // 4
#define JSON_FW_ERR_VERIFY      "VerifyUrl"         // 5
#define JSON_FW_ERR_VERSION     "InvalidVersion"    // 6
#define JSON_FW_ERR_DOWNLOAD    "DownloadError"     // 7
#define JSON_FW_ERR_TIMEOUT     "DownloadTimeout"   // 8
#define JSON_FW_ERR_INSTALL     "InstallFailed"     // 9
#define JSON_FW_ERR_INVALID     "InvalidFirmware"   // 10

// fwup reason(code)
#define JSON_FW_CODE_INTERNAL   1
#define JSON_FW_CODE_NET        2
#define JSON_FW_CODE_FILE       3
#define JSON_FW_CODE_URL        4
#define JSON_FW_CODE_VERIFY     5
#define JSON_FW_CODE_VERSION    6
#define JSON_FW_CODE_DOWNLOAD   7
#define JSON_FW_CODE_TIMEOUT    8
#define JSON_FW_CODE_INSTALL    9
#define JSON_FW_CODE_INVALID    10

typedef struct result_err_tag
{
    int code;
    string desc;

    result_err_tag()
    {
        code = 0;
        desc = "";
    }
} result_err_t;

// request param
typedef struct param_auth_tag
{
    string model;
    string imei;
    string serial_num;

    param_auth_tag()
    {
        model = "";
        imei = "";
        serial_num = "";
    }
} param_auth_t;

typedef struct param_sinfo_tag
{
    string msw;
    string dsw;
    string ctn;
    string band;
    string apn;

    param_sinfo_tag()
    {
        msw = "";
        dsw = "";
        ctn = "";
        band = "";
        apn = "";
    }
} param_sinfo_t;

typedef struct param_sauth_tag
{
    string sinfo;
    string khint;

    param_sauth_tag()
    {
        sinfo = "";
        khint = "";
    }
} param_sauth_t;

typedef struct param_boot_tag
{
    string mdm_ver;
    string dev_ver;
    string timestamp;
    string reason;
    string sinfo;
    string khint;

    param_boot_tag()
    {
        mdm_ver = "";
        dev_ver = "";
        timestamp = "";
        reason = "";
        sinfo = "";
        khint = "";
    }
} param_boot_t;

typedef struct param_event_net_tag
{
    string sts;
    string ets;
    string reason;

    param_event_net_tag()
    {
        sts = "";
        ets = "";
        reason = "";
    }
} param_event_net_t;

typedef struct param_event_ip_tag
{
    string ip;
    string ts;

    param_event_ip_tag()
    {
        ip = "";
        ts = "";
    }
} param_event_ip_t;

typedef struct event_lan_tag
{
    int pn;
    int ps;
    string ts;

    event_lan_tag()
    {
        pn = 0;
        ps = 0;
        ts = "";
    }
} param_ev_t;

typedef struct param_event_lan_tag
{
    param_ev_t port[4];
} param_event_lan_t;

typedef struct param_event_alive_tag
{
    int pci;
    string cell_id;
    int freq;
    int band;
    string ip;
    int rssi;
    int rsrp;
    int rsrq;
    int sinr;
    string uptime;
    string timestamp;
} param_event_alive_t;

typedef struct param_event_tag
{
    string nm;
    param_event_net_t net_err;
    param_event_ip_t ip;
    param_event_lan_t lan;
    param_event_alive_t alive;

    param_event_tag()
    {
        nm = "";
    }
} param_event_t;

typedef struct param_report_tag
{
    int pci;
    string cell_id;
    int freq;
    int band;
    string ip;
    int rssi;
    int rsrp;
    int rsrq;
    int sinr;
    int tx;
    int rx;
    string uptime;
    int reset_cnt;
    int ip_change;
    int err_cnt;
    int umem;
    string timestamp;
    int lan_port[4];

    param_report_tag()
    {
        pci = 0;
        cell_id = "";
        freq = 0;
        band = 0;
        ip = "";
        rssi = 0;
        rsrp = 0;
        rsrq = 0;
        sinr = 99;
        tx = 0;
        rx = 0;
        uptime = "";
        reset_cnt = 0;
        ip_change = 0;
        err_cnt = 0;
        umem = 0;
        timestamp = "";
        memset(lan_port, 0,sizeof(lan_port));
    }
} param_report_t;

typedef struct param_heartbeat_tag
{
    //
} param_heartbeat_t;

typedef struct param_getkey_tag
{
    string sinfo;
    string khint;

    param_getkey_tag()
    {
        sinfo = "";
        khint = "";
    }
} param_getkey_t;

typedef struct param_keyupdated_tag
{
    string khint;

    param_keyupdated_tag()
    {
        khint = "";
    }
} param_keyupdated_t;

typedef struct
{
    //
} param_getparam_t;

typedef struct  param_paramupdated_tag
{
    string pts;
    int ev_flag;
    string band;
    string apn;

    param_paramupdated_tag()
    {
        pts = "";
        ev_flag = 0;
        band = "";
        apn = "";
    }
} param_paramupdated_t;

typedef struct param_getfwup_tag
{
    string ver;

    param_getfwup_tag()
    {
        ver = "";
    }
} param_getfwup_t;

typedef struct param_fwup_tag
{
    string status;
    string fail_reason;
    result_err_t err;

    param_fwup_tag()
    {
        status = "";
        fail_reason = "";
    }
} param_fwup_t;

// response param
typedef struct result_cmds_tag
{
    string key;
    string value;

    result_cmds_tag()
    {
        key = "";
        value = "";
    }
} result_cmds_t;

typedef struct result_dev_param_tag
{
    string key;
    string value_str;
    int value_int;

    result_dev_param_tag()
    {
        key = "";
        value_str = "";
        value_int = 0;
    }
} result_dev_param_t;

typedef struct result_auth_tag
{
    string ret;
    string ts;
    result_err_t err;
    int retry_time;
    list<result_cmds_t> cmds;

    result_auth_tag()
    {
        ret = "";
        ts = "";
        err.code = 0;
        err.desc = "";
        retry_time  = 0;
        cmds.clear();
    }
} result_auth_t;

typedef struct result_sauth_tag
{
    string ret;
    string ts;
    result_err_t err;
    int retry_time;
    list<result_cmds_t> cmds;

    result_sauth_tag()
    {
        ret = "";
        ts = "";
        err.code = 0;
        err.desc = "";
        retry_time = 0;
        cmds.clear();
    }
} result_sauth_t;

typedef struct result_boot_tag
{
    string ret;
    string ts;
    int report_time;
    int heartbeat_time;
    int retry_time;
    list<result_cmds_t> cmds;

    result_boot_tag()
    {
        ret = "";
        ts = "";
        report_time = 0;
        heartbeat_time = 0;
        retry_time = 0;
        cmds.clear();
    }
} result_boot_t;

typedef struct result_event_tag
{
    string ret;
    string ts;

    result_event_tag()
    {
        ret = "";
        ts = "";
    }
} result_event_t;

typedef struct result_report_tag
{
    string ret;
    string ts;
    int report_time;
    int heartbeat_time;
    list<result_cmds_t> cmds;

    result_report_tag()
    {
        ret = "";
        ts = "";
        report_time = 0;
        heartbeat_time = 0;
        cmds.clear();
    }
} result_report_t;

typedef struct
{
    string ts;
    int report_time;
    bool report_set;
    int heartbeat_time;
    bool heartbeat_set;
    list<result_cmds_t> cmds;
} result_heartbeat_t;

typedef struct result_getkey_tat
{
    string ret;
    string cs;
    string skey;
    string ts;

    result_getkey_tat()
    {
        ret = "";
        cs = "";
        skey = "";
        ts = "";
    }
} result_getkey_t;

typedef struct result_keyupdated_tag
{
    string ret;
    string ts;

    result_keyupdated_tag()
    {
        ret = "";
        ts = "";
    }
} result_keyupdated_t;

typedef struct result_getparam_tag
{
    string ret;
    list<result_dev_param_t> params;

    result_getparam_tag()
    {
        ret = "";
        params.clear();
    }
} result_getparam_t;

typedef struct  result_paramupdated_tag
{
    string ret;
    string ts;

    result_paramupdated_tag()
    {
        ret = "";
        ts = "";
    }
} result_paramupdated_t;

typedef struct result_getfwup_tag
{
    string ret;
    string tp;
    string up_ver;
    string url;
    string cs_url;
    string start_tm;
    int span;

    result_getfwup_tag()
    {
        ret = "";
        tp = "";
        up_ver = "";
        url = "";
        cs_url = "";
        start_tm = "";
        span = 0;
    }
} result_getfwup_t;

typedef struct result_fwup_tag
{
    string ret;
    string ts;

    result_fwup_tag()
    {
        ret = "";
        ts = "";
    }
} result_fwup_t;

class json_param
{
public:
    typedef enum
    {
        TYPE_STR =      0,
        TYPE_INT =      1,
        TYPE_NUM =      2
    } json_type_e;

public:
    string key;
    string value_str;
    int value_int;
    double value_num;
    json_type_e type;

    json_param(string key, string value)
    {
        this->key = key;
        value_str = value;
        value_int = 0;
        value_num = 0;
        this->type = TYPE_STR;
    };

    json_param(string key, int value)
    {
        this->key = key;
        value_str = "";
        value_int = value;
        value_num = 0;
        this->type = TYPE_INT;
    };

    json_param(string key, double value)
    {
        this->key = key;
        value_str = "";
        value_int = 0;
        value_num = value;
        this->type = TYPE_NUM;
    };

    void set_param(string key, string value)
    {
        this->key = key;
        value_str = value;
        value_int = 0;
        value_num = 0;
        this->type = TYPE_STR;
    };

    void set_param(string key, int value)
    {
        this->key = key;
        value_str = "";
        value_int = value;
        value_num = 0;
        this->type = TYPE_INT;
    };

    void set_param(string key, double value)
    {
        this->key = key;
        value_str = "";
        value_int = 0;
        value_num = value;
        this->type = TYPE_NUM;
    };

    void set_value(string value)
    {
        value_str = value;
        value_int = 0;
        value_num = 0;
        this->type = TYPE_STR;
    };

    void set_value(int value)
    {
        value_str = "";
        value_int = value;
        value_num = 0;
        this->type = TYPE_INT;
    };

    void set_value(double value)
    {
        value_str = "";
        value_int = 0;
        value_num = value;
        this->type = TYPE_NUM;
    };
};

class mdm_fw_list
{
public:
    string _ver;
    string _nm;
};

class json_manager
{
public:
    json_manager();
    ~json_manager();
    int init(void);
    int deinit(void);

    int config_create(list<json_param> &obj_list, string &json_body);
    int config_parse(string &src_data, list<json_param> &obj_list);

    int ota_metadata_dfd_parser(string &src, string &url, string &m2g2g, string &m4g2g);
    int ota_metadata_mfd_parser(string &src, string &url, list<mdm_fw_list> &fw_list);

    int cloud_body_auth(string &json_body, param_auth_t &param_data);
    int cloud_body_sinfo(string &json_body, param_sinfo_t &param_data);
    int cloud_body_sauth(string &json_body, param_sauth_t &param_data);
    int cloud_body_boot(string &json_body, param_boot_t &param_data);
    int cloud_body_event(string &json_body, param_event_t &param_data, int type);
    int cloud_body_report(string &json_body, param_report_t &param_data);
    int cloud_body_heartbeat(string &json_body, param_heartbeat_t &param_data);
    int cloud_body_getkey(string &json_body, param_getkey_t &param_data);
    int cloud_body_keyupdated(string &json_body, param_keyupdated_t &param_data);
    int cloud_body_getparam(string &json_body, param_getparam_t &param_data);
    int cloud_body_paramupdated(string &json_body, param_paramupdated_t &param_data);
    int cloud_body_getfwup(string &json_body, param_getfwup_t &param_data);
    int cloud_body_fwup(string &json_body, param_fwup_t &param_data);

    int cloud_response_auth(string &recv_body, result_auth_t &result);
    int cloud_response_sauth(string &recv_body, result_sauth_t &result);
    int cloud_response_boot(string &recv_body, result_boot_t &result);
    int cloud_response_event(string &recv_body, result_event_t &result);
    int cloud_response_report(string &recv_body, result_report_t &result);
    int cloud_response_heartbeat(string &recv_body, result_heartbeat_t &result);
    int cloud_response_getkey(string &recv_body, result_getkey_t &result);
    int cloud_response_keyupdated(string &recv_body, result_keyupdated_t &result);
    int cloud_response_getparam(string &recv_body, result_getparam_t &result);
    int cloud_response_paramupdated(string &recv_body, result_paramupdated_t &result);
    int cloud_response_getfwup(string &recv_body, result_getfwup_t &result);
    int cloud_response_fwup(string &recv_body, result_fwup_t &result);
};
#endif
