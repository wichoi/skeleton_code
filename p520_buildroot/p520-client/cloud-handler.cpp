#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <fw-api.h>

#include "common.h"
#include "log.h"
#include "event.h"
#include "timer.h"
#include "utils.h"
#include "config-manager.h"
#include "curl-handler.h"
#include "json-manager.h"
#include "cloud-handler.h"

static void *pthread_cb(void *arg)
{
    cloud_handler *fp_fwup = (cloud_handler*)arg;
    fp_fwup->cloud_proc();
}

cloud_handler::cloud_handler():
    _p_main(),
    _auth_handle(),
    _curl_handle(),
    _json_handle(),
    _resolution(100),
    _state(CLD_START),
    _retry_flag(RETRY_NONE),
    _retry_cnt(0),
    _get_key_flag(0),
    _param_update_flag(0),
    _not_found_flag(0),
    _ev_q(),
    _thread(),
    _mtx(),
    _exit_flag(0)
{
}

cloud_handler::~cloud_handler()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int cloud_handler::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("cloud_handler::%s \n", __func__);
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_NOTIFY_NER_ERROR, this);
    _p_main->event_subscribe(event_c::CMD_NOTIFY_IP_CHANGE, this);
    _p_main->event_subscribe(event_c::CMD_NOTIFY_LINK_CHANGE, this);
    _p_main->event_subscribe(event_c::CMD_NOTIFY_LINK_UP, this);

    _p_main->event_subscribe(event_c::CMD_CLOUD_ST_CLEAR, this);

    _p_main->event_subscribe(event_c::CMD_CLOUD_AUTH, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_SAUTH, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_BOOT, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_EVENT, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_REPORT, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_HEARTBEAT, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_GET_KEY, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_KEY_UPDATED, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_GET_PARAM, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_PARAM_UPDATED, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_GET_FWUP, this);
    _p_main->event_subscribe(event_c::CMD_CLOUD_FWUP, this);

    _p_main->event_subscribe(event_c::CMD_FWUP_CLEAR, this);
    _p_main->event_subscribe(event_c::CMD_FWUP_DOWNLOAD, this);
    _p_main->event_subscribe(event_c::CMD_FWUP_TIME_CALC, this);
    _p_main->event_subscribe(event_c::CMD_FWUP_VERIFY, this);
    _p_main->event_subscribe(event_c::CMD_FWUP_INSTALL, this);

    _auth_handle.init();
    _curl_handle.init();
    _json_handle.init();

    _resolution = resolution;
    _state = CLD_START;
    _retry_flag = RETRY_NONE;
    _retry_cnt = 0;
    _get_key_flag = 0;
    _param_update_flag = 0;
    _not_found_flag = 0;

    utils::update_router_usb0_tx();
    utils::update_router_usb0_rx();
    u32 curr_tx = config_manager::instance()->get_router_tx();
    u32 curr_rx = config_manager::instance()->get_router_rx();
    config_manager::instance()->set_router_prev_tx(curr_tx);
    config_manager::instance()->set_router_prev_rx(curr_rx);

    pthread_mutex_init(&_mtx, NULL);
    if(pthread_create(&_thread, NULL, pthread_cb, (void*)this) < 0)
    {
        log_e("fwup thread create failed !!!\n");
    }
    pthread_detach(_thread);

    return ret_val;
}

int cloud_handler::deinit(void)
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _auth_handle.deinit();
    _curl_handle.deinit();
    _json_handle.deinit();

    pthread_mutex_lock(&_mtx);
    _ev_q.clear();
    pthread_mutex_unlock(&_mtx);
    pthread_mutex_destroy(&_mtx);

    return RET_OK;
}

int cloud_handler::cloud_retry(int retry)
{
    log_i("cloud_standby flag[%d] cnt[%d]\n", retry, _retry_cnt);
    _retry_flag = (retry_e)retry;
    if(_retry_flag == RETRY_NONE)
    {
        _retry_cnt = 0;
    }
    else if(_retry_flag == RETRY_SET)
    {
        _retry_cnt++;
        //u32 msec = _retry_cnt < 3 ? 1000 * 10 : 1000 * 60 * 5; // 10sec, 5min
        u32 msec = config_manager::instance()->get_cloud_retry_interval();
        _p_main->set_timer(timer::TID_CLOUD_RETRY, msec, this);
    }
    else if(_retry_flag == POLLING_SET)
    {
        _retry_cnt = 0;
        u32 msec = config_manager::instance()->get_cloud_report_interval();
        _p_main->set_timer(timer::TID_CLOUD_POLLING, msec, this);
    }
    else if(_retry_flag == WAIT_GET_KEY)
    {
        _retry_cnt++;
    }

    return RET_OK;
}

int cloud_handler::cloud_state(void)
{
    if(_retry_flag == RETRY_SET || _retry_flag == POLLING_SET)
    {
        return RET_OK;
    }

    u32 prev_st = _state;

    switch(_state)
    {
    case CLD_START:
        cloud_retry(RETRY_NONE);
        _state = CLD_INIT;
        break;
    case CLD_INIT:
        cloud_retry(RETRY_NONE);
        _state = CLD_NET_CHECK;
        break;
    case CLD_NET_CHECK:
        {
            string ip_addr = "";
            utils::read_ip_addr(ip_addr);
            log_v("ip[%s]\n", ip_addr.c_str());
            if(ip_addr.length() > 0)
            {
                string timestamp;
                utils::read_timestamp(timestamp);
                config_manager::instance()->set_cloud_recovery_time(timestamp);
                cloud_retry(RETRY_NONE);
                _state = CLD_AUTH;
            }
            else
            {
                usleep(10 * 1000 * 1000); // 10sec
            }
        }
        break;
    case CLD_AUTH:
        {
            string server_token = config_manager::instance()->get_cloud_token();
            if(server_token.length() > 0)
            {
                cloud_retry(RETRY_NONE);
                _state = CLD_GET_KEY;
            }
            else
            {
                if(cloud_sauth() == RET_OK)
                {
                     cloud_retry(RETRY_NONE);
                     _state = CLD_GET_KEY;
                }
                else
                {
                    cloud_retry(RETRY_SET);
                }
            }
        }
       break;
    case CLD_GET_KEY:
        if(_get_key_flag == 0)
        {
            cloud_retry(RETRY_NONE);
            _state = CLD_BOOT;
        }
        else
        {
            if(_retry_cnt > 5)
            {
                cloud_retry(RETRY_NONE);
                _state = CLD_BOOT;
            }
            else
            {
                usleep(10 * 1000 * 1000); // 10sec
                cloud_retry(WAIT_GET_KEY);
            }
        }
        break;
    case CLD_BOOT:
        if(cloud_boot() == RET_OK)
        {
            string reboot_reason = config_manager::instance()->get_cloud_reboot_reason();
            string fw_type = config_manager::instance()->get_cloud_fw_device();
            string fw_ver = config_manager::instance()->get_cloud_fw_version();
            if(reboot_reason.compare("NetworkError") == 0)
            {
                _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_NER_ERR);
            }

            if(config_manager::instance()->get_cloud_fw_status().length() > 0)
            {
                string fw_status = JSON_FW_ST_INSTALLED;
                string fw_reason = "";
                int fw_code = 0;
                string fw_desc = "OK";

                if(reboot_reason.compare("FirmwareUpdate") == 0)
                {
                    string dev_ver = config_manager::instance()->get_router_ver_ex();
                    string mdm_ver = config_manager::instance()->get_modem_ver_ex();
                    // todo modem upgrade
                    if(fw_type.compare("Device") == 0)
                    {
                        if(fw_ver.compare(dev_ver) != 0)
                        {
                            fw_status = JSON_FW_ST_FAILED;
                            fw_reason = JSON_FW_ERR_INSTALL;
                            fw_code = JSON_FW_CODE_INSTALL;
                            fw_desc = "install failed.";
                        }
                    }
                    else if(fw_type.compare("Modem") == 0)
                    {
                        if(fw_ver.compare(mdm_ver) != 0)
                        {
                            fw_status = JSON_FW_ST_FAILED;
                            fw_reason = JSON_FW_ERR_INSTALL;
                            fw_code = JSON_FW_CODE_INSTALL;
                            fw_desc = "install failed.";
                        }
                    }
                    else if(fw_type.compare("Total") == 0)
                    {
                        string total_ver = dev_ver + "-" + mdm_ver;
                        if(total_ver.compare(total_ver) != 0)
                        {
                            fw_status = JSON_FW_ST_FAILED;
                            fw_reason = JSON_FW_ERR_INSTALL;
                            fw_code = JSON_FW_CODE_INSTALL;
                            fw_desc = "install failed.";
                        }
                    }
                }
                else
                {
                    fw_status = JSON_FW_ST_FAILED;
                    fw_reason = JSON_FW_ERR_INSTALL;
                    fw_code = JSON_FW_CODE_INSTALL;
                    fw_desc = "device reboot.";
                }

                _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
                config_manager::instance()->set_cloud_fw_status(fw_status);
                config_manager::instance()->set_cloud_fw_reason(fw_reason);
                config_manager::instance()->set_cloud_fw_code(fw_code);
                config_manager::instance()->set_cloud_fw_description(fw_desc);
                _p_main->event_publish(event_c::CMD_CLOUD_FWUP);
            }

            reboot_reason = "Unknown";
            config_manager::instance()->set_cloud_reboot_reason(reboot_reason);

            fw_type = "";
            config_manager::instance()->set_cloud_fw_device(fw_type);

            fw_ver = "";
            config_manager::instance()->set_cloud_fw_version(fw_ver);

            cloud_retry(RETRY_NONE);
            _state = CLD_POLLING;
        }
        else
        {
            cloud_retry(RETRY_SET);
        }
        break;
    case CLD_POLLING:
        if(cloud_report() == RET_OK)
        {
            cloud_retry(POLLING_SET);
            //_state = CLD_END;
        }
        else
        {
            cloud_retry(POLLING_SET);
            //cloud_retry(RETRY_SET);
        }
        break;
    case CLD_END:
        {
            //log_i("cloud_state CLD_END !!!\n");
            sleep(10);
        }
        break;
    default:
        break;
    }

    if(prev_st != _state)
    {
        string str_st = "UNKNOWN";
        if(_state == CLD_START)             str_st = "CLD_START";
        else if(_state == CLD_INIT)         str_st = "CLD_INIT";
        else if(_state == CLD_NET_CHECK)    str_st = "CLD_NET_CHECK";
        else if(_state == CLD_AUTH)         str_st = "CLD_AUTH";
        else if(_state == CLD_GET_KEY)      str_st = "CLD_GET_KEY";
        else if(_state == CLD_BOOT)         str_st = "CLD_BOOT";
        else if(_state == CLD_POLLING)      str_st = "CLD_POLLING";
        else if(_state == CLD_END)          str_st = "CLD_END";
        log_i("cloud_state[%d][%s]\n", _state, str_st.c_str());
    }

    return RET_OK;
}

int cloud_handler::cloud_event(void)
{
    event_c ev;
    ev._cmd = event_c::CMD_NONE;
    pthread_mutex_lock(&_mtx);
    if(_ev_q.size() > 0)
    {
        log_d("%s : %u, op_code : %u \n",
                __func__, _ev_q.front()._cmd, _ev_q.front()._op_code);
        ev = _ev_q.front();
        _ev_q.pop_front();
    }
    pthread_mutex_unlock(&_mtx);

    switch(ev._cmd)
    {
    case event_c::CMD_NOTIFY_NER_ERROR:
        log_i("%s CMD_NOTIFY_NER_ERROR\n", __func__);
#if 0
        if(config_manager::instance()->get_cloud_notify_net_err())
        {
            _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_NER_ERR);
        }
#endif
        break;
    case event_c::CMD_NOTIFY_IP_CHANGE:
        log_i("%s CMD_NOTIFY_IP_CHANGE\n", __func__);
        if(config_manager::instance()->get_cloud_notify_ip_change())
        {
            _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_IP_CHANGE);
        }
        break;
    case event_c::CMD_NOTIFY_LINK_CHANGE:
        log_i("%s CMD_NOTIFY_LINK_CHANGE\n", __func__);
        break;
    case event_c::CMD_NOTIFY_LINK_UP:
        log_i("%s CMD_NOTIFY_LINK_UP\n", __func__);
        break;
    case event_c::CMD_CLOUD_ST_CLEAR:
        cloud_st_clear();
        break;
    case event_c::CMD_CLOUD_AUTH:
        cloud_auth();
        break;
    case event_c::CMD_CLOUD_SAUTH:
        cloud_sauth();
        break;
    case event_c::CMD_CLOUD_BOOT:
        cloud_boot();
        break;
    case event_c::CMD_CLOUD_EVENT:
        cloud_event_inform(ev);
        break;
    case event_c::CMD_CLOUD_REPORT:
        cloud_report();
        break;
    case event_c::CMD_CLOUD_HEARTBEAT:
        cloud_heartbeat();
        break;
    case event_c::CMD_CLOUD_GET_KEY:
        cloud_getkey();
        break;
    case event_c::CMD_CLOUD_KEY_UPDATED:
        cloud_keyupdated();
        break;
    case event_c::CMD_CLOUD_GET_PARAM:
        cloud_getparam();
        break;
    case event_c::CMD_CLOUD_PARAM_UPDATED:
        cloud_paramupdated();
        break;
    case event_c::CMD_CLOUD_GET_FWUP:
        cloud_getfwup();
        break;
    case event_c::CMD_CLOUD_FWUP:
        cloud_fwup();
        break;
    case event_c::CMD_FWUP_CLEAR:
        fwup_clear();
        break;
    case event_c::CMD_FWUP_DOWNLOAD:
        fwup_download();
        break;
    case event_c::CMD_FWUP_TIME_CALC:
        fwup_time_calculation();
        break;
    case event_c::CMD_FWUP_VERIFY:
        fwup_verify();
        break;
    case event_c::CMD_FWUP_INSTALL:
        fwup_install();
        break;
    default:
        break;
    }

    return RET_OK;
}

int cloud_handler::cloud_proc(void)
{
    while(_exit_flag == 0)
    {
        cloud_state();
        cloud_event();
        usleep(_resolution * 1000);
    }
    log_d("%s exit \n", __func__);
    return RET_OK;
}

int cloud_handler::get_queue_cnt(void)
{
    return (int)(_ev_q.size());
}

int cloud_handler::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    //log_i("cloud_handler::%s cmd[%d]\n", __func__, ev._cmd);
    pthread_mutex_lock(&_mtx);
    if(_ev_q.size() < event_queue::QUE_MAX)
    {
        _ev_q.push_back(ev);
    }
    else
    {
        log_w("%s queue_full[%d] \n", __func__, event_queue::QUE_MAX);
        exit(1);
    }
    pthread_mutex_unlock(&_mtx);
    return ret_val;
}

int cloud_handler::on_timer(u32 id)
{
    log_v("%s id[%u] \n", __func__, id);
    switch(id)
    {
    case timer::TID_CLOUD_RETRY:
        _p_main->kill_timer(timer::TID_CLOUD_RETRY);
        cloud_retry(RETRY_EXECUTE);
        break;
    case timer::TID_CLOUD_POLLING:
        _p_main->kill_timer(timer::TID_CLOUD_POLLING);
        cloud_retry(POLLING_EXECUTE);
        break;
    case timer::TID_CLOUD_HEARTBEAT:
        _p_main->event_publish(event_c::CMD_CLOUD_HEARTBEAT);
        break;
    case timer::TID_CLOUD_PARAM_UPDATED:
        _p_main->kill_timer(timer::TID_CLOUD_PARAM_UPDATED);
        _p_main->event_publish(event_c::CMD_CLOUD_PARAM_UPDATED);
        break;
    case timer::TID_CLOUD_FWUP_VERIFY:
        _p_main->kill_timer(timer::TID_CLOUD_FWUP_VERIFY);
        _p_main->event_publish(event_c::CMD_FWUP_VERIFY);
        break;
    default:
        break;
    }

    return RET_OK;
}

int cloud_handler::cloud_st_clear(void)
{
    log_i("%s\n", __func__);
    string empty = "";
    config_manager::instance()->set_cloud_token(empty);
    config_manager::instance()->set_cloud_private_key(empty);

    _state = CLD_START;
    _retry_flag = RETRY_NONE;
    _retry_cnt = 0;
    _get_key_flag = 0;
    _param_update_flag = 0;

    return RET_OK;
}

int cloud_handler::cloud_auth(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/auth");

    string json_body = "";
    param_auth_t param_data;
    param_data.model = config_manager::instance()->get_router_model();
    param_data.imei = config_manager::instance()->get_modem_imei();
    param_data.serial_num = config_manager::instance()->get_router_serial();
    _json_handle.cloud_body_auth(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    if(_curl_handle.curl_request(server_url, json_body, method, resp) == RET_OK)
    {
        utils::http_header_parser(resp.header);
        if(resp.code == 200)
        {
            result_auth_t result;
            if(_json_handle.cloud_response_auth(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s] err_code[%d] err_str[%s] ret_tm[%d] \n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str(),
                                            result.err.code,
                                            result.err.desc.c_str(),
                                            result.retry_time);
                list<result_cmds_t>::iterator iter;
                for(iter = result.cmds.begin(); iter != result.cmds.end(); ++iter)
                {
                    log_i("%s cmds key[%s] value[%s]\n", __func__,
                                    iter->key.c_str(), iter->value.c_str());
                }

                set_retry_time(result.retry_time);

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    execute_cloud_cmd(result.cmds);
                }
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_sauth(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/sauth");

    string json_body = "";
    param_sauth_t param_data;
    make_secure_info(param_data.sinfo, param_data.khint);
    _json_handle.cloud_body_sauth(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    if(_curl_handle.curl_request(server_url, json_body, method, resp) == RET_OK)
    {
        utils::http_header_parser(resp.header);
        if(resp.code == 200)
        {
            result_sauth_t result;
            if(_json_handle.cloud_response_sauth(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s] err_code[%d] err_str[%s] ret_tm[%d]\n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str(),
                                            result.err.code,
                                            result.err.desc.c_str(),
                                            result.retry_time);
                list<result_cmds_t>::iterator iter;
                for(iter = result.cmds.begin(); iter != result.cmds.end(); ++iter)
                {
                    log_i("%s cmds key[%s] value[%s]\n", __func__,
                                    iter->key.c_str(), iter->value.c_str());
                }

                set_retry_time(result.retry_time);

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    execute_cloud_cmd(result.cmds);
                }
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_boot(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string timestamp = "";
    utils::read_timestamp(timestamp);

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/boot");

    string json_body = "";
    param_boot_t param_data;
    make_secure_boot(param_data.sinfo, param_data.khint);
    param_data.mdm_ver = config_manager::instance()->get_modem_version();
    param_data.dev_ver = config_manager::instance()->get_router_version();
    param_data.timestamp = timestamp;
    param_data.reason = config_manager::instance()->get_cloud_reboot_reason();
    _json_handle.cloud_body_boot(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        utils::http_header_parser(resp.header);
        if(resp.code == 200)
        {
            result_boot_t result;
            if(_json_handle.cloud_response_boot(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s] report[%d], heartbeat[%d] ret_tm[%d]\n",
                                            __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str(),
                                            result.report_time,
                                            result.heartbeat_time,
                                            result.retry_time);
                list<result_cmds_t>::iterator iter;
                for(iter = result.cmds.begin(); iter != result.cmds.end(); ++iter)
                {
                    log_i("%s cmds key[%s] value[%s]\n", __func__,
                                    iter->key.c_str(), iter->value.c_str());
                }

                set_retry_time(result.retry_time);

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    set_report_time(result.report_time);
                    set_heartbeat_time(result.heartbeat_time);
                    execute_cloud_cmd(result.cmds);
                }
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_event_inform(event_c &ev)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string timestamp = "";
    utils::read_timestamp(timestamp);

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/event");

    string json_body = "";
    param_event_t param_data;

    if(ev._op_code == event_c::OP_NER_ERR)
    {
        param_data.nm = JSON_OBJ_NER_ERR;
        param_data.net_err.sts = config_manager::instance()->get_cloud_reboot_time();
        param_data.net_err.ets = config_manager::instance()->get_cloud_recovery_time();
        param_data.net_err.reason = "DevReset"; // MdmReset(todo)
    }
    else if(ev._op_code == event_c::OP_IP_CHANGE)
    {
        param_data.nm = JSON_OBJ_IP_CHANGE;
        param_data.ip.ip = config_manager::instance()->get_modem_ip_addr();
        param_data.ip.ts = timestamp;
    }
    else if(ev._op_code == event_c::OP_LAN_CHANGE)
    {
        // todo
        param_data.nm = JSON_OBJ_LAN_CHANGE;
        param_data.lan.port[0].pn = 0;
        param_data.lan.port[0].ps = 0;
        param_data.lan.port[0].ts = "";

        param_data.lan.port[1].pn = 0;
        param_data.lan.port[1].ps = 0;
        param_data.lan.port[1].ts = "";

        param_data.lan.port[2].pn = 0;
        param_data.lan.port[2].ps = 0;
        param_data.lan.port[2].ts = "";

        param_data.lan.port[3].pn = 0;
        param_data.lan.port[3].ps = 0;
        param_data.lan.port[3].ts = "";

    }
    else // if(ev._op_code == event_c::OP_SMS_ALIVE)
    {
        param_data.nm = JSON_OBJ_SMS_ALIVE;
        param_data.alive.pci = config_manager::instance()->get_modem_pci();
        param_data.alive.cell_id = config_manager::instance()->get_modem_cell_id();
        param_data.alive.freq = config_manager::instance()->get_modem_frequency();
        param_data.alive.band = config_manager::instance()->get_modem_band();
        param_data.alive.ip = config_manager::instance()->get_modem_ip_addr();
        param_data.alive.rssi = config_manager::instance()->get_modem_rssi();
        param_data.alive.rsrp = config_manager::instance()->get_modem_rsrp();
        param_data.alive.rsrq = config_manager::instance()->get_modem_rsrq();
        param_data.alive.sinr = config_manager::instance()->get_modem_sinr();
        param_data.alive.uptime = config_manager::instance()->get_router_uptime();
        param_data.alive.timestamp = timestamp;
    }

    _json_handle.cloud_body_event(json_body, param_data, ev._op_code);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        utils::http_header_parser(resp.header);
        if(resp.code == 200)
        {
            result_event_t result;
            if(_json_handle.cloud_response_event(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s]\n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str());

                ret_val = response_result(result.ret);
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_report(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string timestamp = "";
    utils::read_timestamp(timestamp);

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/report");

    string mem = "";
    int umem = utils::read_mem_usage(mem);

    u32 curr_tx = config_manager::instance()->get_router_tx();
    u32 curr_rx = config_manager::instance()->get_router_rx();
    u32 prev_tx = config_manager::instance()->get_router_prev_tx();
    u32 prev_rx = config_manager::instance()->get_router_prev_rx();
    u32 report_tx = (curr_tx >= prev_tx) ? (curr_tx - prev_tx) : curr_tx;
    u32 report_rx = (curr_rx >= prev_rx) ? (curr_rx - prev_rx) : curr_rx;

    string json_body = "";
    param_report_t param_data;
    param_data.pci = config_manager::instance()->get_modem_pci();
    param_data.cell_id = config_manager::instance()->get_modem_cell_id();
    param_data.freq = config_manager::instance()->get_modem_frequency();
    param_data.band = config_manager::instance()->get_modem_band();
    param_data.ip = config_manager::instance()->get_modem_ip_addr();
    param_data.rssi = config_manager::instance()->get_modem_rssi();
    param_data.rsrp = config_manager::instance()->get_modem_rsrp();
    param_data.rsrq = config_manager::instance()->get_modem_rsrq();
    param_data.sinr = config_manager::instance()->get_modem_sinr();
    param_data.tx = report_tx;
    param_data.rx = report_rx;
    param_data.uptime = config_manager::instance()->get_router_uptime();
    param_data.reset_cnt = config_manager::instance()->get_modem_reset_cnt();
    param_data.ip_change = config_manager::instance()->get_ip_change_cnt();
    param_data.err_cnt = config_manager::instance()->get_network_err_cnt();
    param_data.umem = umem;
    param_data.timestamp = timestamp;
    // todo param_data.lan_port;
    _json_handle.cloud_body_report(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        utils::http_header_parser(resp.header);
        if(resp.code == 200)
        {
            result_report_t result;
            if(_json_handle.cloud_response_report(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s] report[%d] heartbeat[%d]\n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str(),
                                            result.report_time,
                                            result.heartbeat_time);
                list<result_cmds_t>::iterator iter;
                for(iter = result.cmds.begin(); iter != result.cmds.end(); ++iter)
                {
                    log_i("%s cmds key[%s] value[%s]\n", __func__,
                                    iter->key.c_str(), iter->value.c_str());
                }

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    config_manager::instance()->set_router_prev_tx(curr_tx);
                    config_manager::instance()->set_router_prev_rx(curr_rx);
                    set_report_time(result.report_time);
                    set_heartbeat_time(result.heartbeat_time);
                    execute_cloud_cmd(result.cmds);
                    _not_found_flag = 0;
                }
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_heartbeat(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/heartb");

    string json_body = "";
    param_heartbeat_t param_data;
    _json_handle.cloud_body_heartbeat(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        utils::http_header_parser(resp.header);
        if(resp.code == 200)
        {
            result_heartbeat_t result;
            if(_json_handle.cloud_response_heartbeat(resp.body, result) == RET_OK)
            {
                log_i("%s ts[%s] report[%d][%d] heartbeat[%d][%d]\n", __func__,
                                            result.ts.c_str(),
                                            result.report_time,
                                            result.report_set,
                                            result.heartbeat_time,
                                            result.heartbeat_set);
                list<result_cmds_t>::iterator iter;
                for(iter = result.cmds.begin(); iter != result.cmds.end(); ++iter)
                {
                    log_i("%s cmds key[%s] value[%s]\n", __func__,
                                    iter->key.c_str(), iter->value.c_str());
                }

                if(result.report_set == true)
                {
                    set_report_time(result.report_time);
                }

                if(result.heartbeat_set == true)
                {
                    set_heartbeat_time(result.heartbeat_time);
                }

                execute_cloud_cmd(result.cmds);
                ret_val = RET_OK;
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_getkey(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/getkey");

    string json_body = "";
    param_getkey_t param_data;
    make_secure_info(param_data.sinfo, param_data.khint);
    _json_handle.cloud_body_getkey(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        if(resp.code == 200)
        {
            result_getkey_t result;
            if(_json_handle.cloud_response_getkey(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] cs[%s] skey[%s] ts[%s]\n", __func__,
                                            result.ret.c_str(),
                                            result.cs.c_str(),
                                            result.skey.c_str(),
                                            result.ts.c_str());

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    decrypt_secure_key(result.cs, result.skey);
                    _get_key_flag = 0;
                }

                _p_main->event_publish(event_c::CMD_CLOUD_KEY_UPDATED);
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_keyupdated(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/keyupdated");

    string json_body = "";
    string sinfo = "";
    param_keyupdated_t param_data;
    make_secure_info(sinfo, param_data.khint);
    _json_handle.cloud_body_keyupdated(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        if(resp.code == 200)
        {
            result_keyupdated_t result;
            if(_json_handle.cloud_response_keyupdated(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s]\n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str());

                ret_val = response_result(result.ret);
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_getparam(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/getparam");

    string json_body = "";
    param_getparam_t param_data;
    _json_handle.cloud_body_getparam(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        if(resp.code == 200)
        {
            result_getparam_t result;
            if(_json_handle.cloud_response_getparam(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s]\n", __func__, result.ret.c_str());
                list<result_dev_param_t>::iterator iter;
                for(iter = result.params.begin(); iter != result.params.end(); ++iter)
                {
                    log_i("%s cmds key[%s] value[%s][%d]\n", __func__,
                                    iter->key.c_str(),
                                    iter->value_str.c_str(),
                                    iter->value_int);
                }

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    set_cloud_param(result.params);
                }

                _p_main->event_publish(event_c::CMD_CLOUD_PARAM_UPDATED);
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_paramupdated(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/paramupdated");

    string timestamp = "";
    utils::read_timestamp(timestamp);

    u32 net_flag = config_manager::instance()->get_cloud_notify_net_err();
    u32  ip_flag = config_manager::instance()->get_cloud_notify_ip_change();
    u32 lan_flag = config_manager::instance()->get_cloud_notify_lan_change();
    u32 ev_flag = net_flag | ip_flag | lan_flag;

    string json_body = "";
    param_paramupdated_t param_data;
    param_data.pts = timestamp;
    param_data.ev_flag = (int)ev_flag;
    param_data.band = config_manager::instance()->get_modem_band_change();
    param_data.apn = config_manager::instance()->get_modem_apn();
    _json_handle.cloud_body_paramupdated(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        if(resp.code == 200)
        {
            result_paramupdated_t result;
            if(_json_handle.cloud_response_paramupdated(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s]\n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str());

                ret_val = response_result(result.ret);
            }
        }
        _param_update_flag = 0;
    }
    else
    {
        if(++_param_update_flag < 3)
        {
            if(_param_update_flag == 1)
            {
                _p_main->set_timer(timer::TID_CLOUD_PARAM_UPDATED, 30 * 1000, this);
            }
            else
            {
                _p_main->set_timer(timer::TID_CLOUD_PARAM_UPDATED, 60 * 1000, this);
            }
        }
        else
        {
            _param_update_flag = 0;
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_getfwup(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/getfwup");

    string json_body = "";
    param_getfwup_t param_data;
    param_data.ver = config_manager::instance()->get_router_ver_ex();
    _json_handle.cloud_body_getfwup(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        if(resp.code == 200)
        {
            result_getfwup_t result;
            if(_json_handle.cloud_response_getfwup(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] tp[%s] up_ver[%s] url[%s] cs_url[%s] start_tm[%s] span[%d]\n", __func__,
                                            result.ret.c_str(),
                                            result.tp.c_str(),
                                            result.up_ver.c_str(),
                                            result.url.c_str(),
                                            result.cs_url.c_str(),
                                            result.start_tm.c_str(),
                                            result.span);

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    fwup_prepare(result);
                }
           }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::cloud_fwup(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    utils::update_modem_info();
    utils::update_router_info();

    string server_url = config_manager::instance()->get_cloud_url();
    server_url.append("/v10/fwup");

    string json_body = "";
    param_fwup_t param_data;
    param_data.status = config_manager::instance()->get_cloud_fw_status();
    param_data.fail_reason = config_manager::instance()->get_cloud_fw_reason();
    param_data.err.code = config_manager::instance()->get_cloud_fw_code();
    param_data.err.desc = config_manager::instance()->get_cloud_fw_description();
    _json_handle.cloud_body_fwup(json_body, param_data);

    curl_handler::method_e method = curl_handler::CMD_POST;
    curl_handler::response resp;
    string server_token = config_manager::instance()->get_cloud_token();
    if(_curl_handle.curl_request(server_url, json_body, method, resp, server_token) == RET_OK)
    {
        if(resp.code == 200)
        {
            result_fwup_t result;
            if(_json_handle.cloud_response_fwup(resp.body, result) == RET_OK)
            {
                log_i("%s ret[%s] ts[%s]\n", __func__,
                                            result.ret.c_str(),
                                            result.ts.c_str());

                ret_val = response_result(result.ret);
                if(ret_val == RET_OK)
                {
                    if(param_data.status.compare(JSON_FW_ST_FAILED) == 0)
                    {
                        _p_main->event_publish(event_c::CMD_FWUP_CLEAR);
                    }
                }
            }
        }
    }

    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::make_secure_number(string &secure_num)
{
    int ret_val = RET_OK;
    string src_num = config_manager::instance()->get_modem_number();
    string key = config_manager::instance()->get_cloud_private_key();
    if(key.empty())
    {
        key.append((const char*)_auth_handle.get_factory_key());
    }

    _auth_handle.ase128_encrypt_base64(secure_num, src_num, key);
    log_d("%s number\n%s\n", __func__, src_num.c_str());
    log_d("%s key[%s]\n", __func__, key.c_str());

    // decrypt test log
    string decrypt = "";
    _auth_handle.ase128_decrypt_base64(decrypt, secure_num, key);
    log_d("%s decrypt[%s]\n", __func__, decrypt.c_str());

    log_i("%s ret_val[%d] [%s]\n", __func__, ret_val, secure_num.c_str());
    return ret_val;
}

int cloud_handler::make_secure_info(string &sinfo, string &khint)
{
    int ret_val = RET_OK;
    string json_body = "";
    param_auth_t param_data;
    param_data.model = config_manager::instance()->get_router_model();
    param_data.imei = config_manager::instance()->get_modem_imei();
    param_data.serial_num = config_manager::instance()->get_router_serial();
    _json_handle.cloud_body_auth(json_body, param_data);

    string key = config_manager::instance()->get_cloud_private_key();
    if(key.empty())
    {
        key.append((const char*)_auth_handle.get_factory_key());
    }
    _auth_handle.ase128_encrypt_base64(sinfo, json_body, key);
    log_d("%s json_body\n%s\n", __func__, json_body.c_str());
    log_d("%s key[%s]\n", __func__, key.c_str());
    log_d("%s sinfo[%s]\n", __func__, sinfo.c_str());

    // decrypt test log
    string decrypt = "";
    _auth_handle.ase128_decrypt_base64(decrypt, sinfo, key);
    log_d("%s decrypt[%s]\n", __func__, decrypt.c_str());

    string hash = "";
    //string base64 = "";
    _auth_handle.sha256_hex_string(hash, key);
    //utils::encode_base64(base64, hash);
    int n = hash.size();
    khint = hash.substr(n-16, 16);
    log_d("%s hash[%s]\n", __func__, hash.c_str());
    //log_d("%s base64[%s]\n", __func__, base64.c_str());
    log_d("%s khint[%s]\n", __func__, khint.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::make_secure_boot(string &sinfo, string &khint)
{
    int ret_val = RET_OK;
    string json_body = "";
    param_sinfo_t param_data;
    param_data.msw = config_manager::instance()->get_modem_ver_ex();
    param_data.dsw = config_manager::instance()->get_router_ver_ex();
    param_data.ctn = config_manager::instance()->get_modem_number();
    param_data.band = config_manager::instance()->get_modem_band_change();
    param_data.apn = config_manager::instance()->get_modem_apn();

    _json_handle.cloud_body_sinfo(json_body, param_data);

    string key = config_manager::instance()->get_cloud_private_key();
    if(key.empty())
    {
        key.append((const char*)_auth_handle.get_factory_key());
    }
    _auth_handle.ase128_encrypt_base64(sinfo, json_body, key);
    log_d("%s json_body\n%s\n", __func__, json_body.c_str());
    log_d("%s key[%s]\n", __func__, key.c_str());
    log_d("%s sinfo[%s]\n", __func__, sinfo.c_str());

    // decrypt test log
    string decrypt = "";
    _auth_handle.ase128_decrypt_base64(decrypt, sinfo, key);
    log_d("%s decrypt[%s]\n", __func__, decrypt.c_str());

    string hash = "";
    //string base64 = "";
    _auth_handle.sha256_hex_string(hash, key);
    //utils::encode_base64(base64, hash);
    int n = hash.size();
    khint = hash.substr(n-16, 16);
    log_d("%s hash[%s]\n", __func__, hash.c_str());
    //log_d("%s base64[%s]\n", __func__, base64.c_str());
    log_d("%s khint[%s]\n", __func__, khint.c_str());

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::decrypt_secure_cmd(string &dest_cmd, string &secure_cmd)
{
    int ret_val = RET_OK;
    log_i("%s secure_cmd[%s]\n", __func__, secure_cmd.c_str());

    string key = config_manager::instance()->get_cloud_private_key();
    if(key.empty())
    {
        key.append((const char*)_auth_handle.get_factory_key());
    }

    _auth_handle.ase128_decrypt_base64(dest_cmd, secure_cmd, key);
    log_i("%s ret_val[%d] [%s]\n", __func__, ret_val, dest_cmd.c_str());
    return ret_val;
}

int cloud_handler::decrypt_secure_key(string &cs, string &skey)
{
    int ret_val = RET_OK;
    string hash = "";
    utils::hex_to_string(hash, skey); // todo

    string key = config_manager::instance()->get_cloud_private_key();
    if(key.empty())
    {
        key.append((const char*)_auth_handle.get_factory_key());
    }

    string decrypt = "";
    _auth_handle.ase128_decrypt_base64(decrypt, skey, key);
    config_manager::instance()->set_cloud_private_key(decrypt);

    log_i("%s ret_val[%d][%s]\n", __func__, ret_val, decrypt.c_str());
    return ret_val;
}

int cloud_handler::set_report_time(int interval) // sec
{
    int ret_val = RET_ERROR;
    if(interval >= 60)
    {
        u32 msec = interval * 1000;
        config_manager::instance()->set_cloud_report_interval(msec);
        cloud_retry(POLLING_SET);
        ret_val = RET_OK;
    }
    else
    {
        log_w("%s %dsec is not applied. min interval is 60 sec !!!\n", __func__, interval);
    }

    log_i("%s ret_val[%d] interval[%d]\n", __func__, ret_val, interval);
    return ret_val;
}

int cloud_handler::set_heartbeat_time(int interval)
{
    int ret_val = RET_ERROR;
    if(interval == 0)
    {
        config_manager::instance()->set_cloud_heartbeat_interval(0);
        _p_main->kill_timer(timer::TID_CLOUD_HEARTBEAT);
        ret_val = RET_OK;
    }
    else if(interval >= 60)
    {
        u32 msec = interval * 1000;
        config_manager::instance()->set_cloud_heartbeat_interval(msec);
        _p_main->set_timer(timer::TID_CLOUD_HEARTBEAT, msec, this);
        ret_val = RET_OK;
    }
    else
    {
        log_w("%s %d sec is not applied. min interval is 60 sec !!!\n", __func__, interval);
    }

    log_i("%s ret_val[%d] interval[%d]\n", __func__, ret_val, interval);
    return ret_val;
}

int cloud_handler::set_retry_time(int interval)
{
    int ret_val = RET_ERROR;
    u32 msec = 1000 * 60 * 60 * 2; // 2hour
    
    if(interval >= 60) // 60sec
    {
        msec = interval * 1000;
        ret_val = RET_OK;
    }

    config_manager::instance()->set_cloud_retry_interval(msec);
    log_i("%s ret_val[%d] interval[%d]\n", __func__, ret_val, interval);
    return ret_val;
}

int cloud_handler::set_cloud_param(list<result_dev_param_t> &params)
{
    int ret_val = RET_OK;
    list<result_dev_param_t>::iterator iter;
    for(iter = params.begin(); iter != params.end(); ++iter)
    {
        if(strcmp(iter->key.c_str(), JSON_PARAM_PTS) == 0)
        {
            // todo
        }
        else if(strcmp(iter->key.c_str(), JSON_PARAM_RPT_INTV) == 0)
        {
            set_report_time(iter->value_int);
        }
        else if(strcmp(iter->key.c_str(), JSON_PARAM_HB_INTV) == 0)
        {
            set_heartbeat_time(iter->value_int);
        }
        else if(strcmp(iter->key.c_str(), JSON_PARAM_EV_FLAGS) == 0)
        {
            u32 net_flag = iter->value_int & 0x0000000000000001;
            u32  ip_flag = iter->value_int & 0x0000000000000010;
            u32 lan_flag = iter->value_int & 0x0000000000000100;
            config_manager::instance()->set_cloud_notify_net_err(net_flag);
            config_manager::instance()->set_cloud_notify_ip_change(ip_flag);
            config_manager::instance()->set_cloud_notify_lan_change(lan_flag);
        }
        else if(strcmp(iter->key.c_str(), JSON_PARAM_BAND) == 0)
        {
            if(utils::set_modem_band(iter->value_str) == RET_OK)
            {
                _p_main->event_publish(event_c::CMD_MODEM_RESET);
            }
        }
        else if(strcmp(iter->key.c_str(), JSON_PARAM_APN) == 0)
        {
            utils::set_modem_apn(iter->value_str);
        }
    }

    return ret_val;
}

int cloud_handler::execute_cloud_cmd(list<result_cmds_t> &cmds)
{
    int ret_val = RET_OK;
    list<result_cmds_t>::iterator iter;
    for(iter = cmds.begin(); iter != cmds.end(); ++iter)
    {
        if(strcmp(iter->key.c_str(), JSON_OBJ_KEY_NM) == 0)
        {
            if(strcmp(iter->value.c_str(), JSON_OBJ_UPDATE_KEY) == 0)
            {
                string private_key = "";
                config_manager::instance()->set_cloud_private_key(private_key);
                _get_key_flag = 1;
                _p_main->event_publish(event_c::CMD_CLOUD_GET_KEY);
            }
            else if(strcmp(iter->value.c_str(), JSON_OBJ_UPDATE_AUTH) == 0)
            {
                string server_token = "";
                config_manager::instance()->set_cloud_token(server_token);
                _p_main->event_publish(event_c::CMD_CLOUD_SAUTH);
            }
            else if(strcmp(iter->value.c_str(), JSON_OBJ_UPDATE_PARAM) == 0)
            {
                _p_main->event_publish(event_c::CMD_CLOUD_GET_PARAM);
            }
            else if(strcmp(iter->value.c_str(), JSON_OBJ_RESET) == 0)
            {
                string reboot_reason = "RemoteReset";
                config_manager::instance()->set_cloud_reboot_reason(reboot_reason);

                u32 cnt = config_manager::instance()->get_modem_reset_cnt();
                config_manager::instance()->set_modem_reset_cnt(cnt);

                _p_main->event_publish(event_c::CMD_REBOOT);
            }
            else if(strcmp(iter->value.c_str(), JSON_OBJ_SYS_CMD) == 0)
            {
            }
            else if(strcmp(iter->value.c_str(), JSON_OBJ_UPDATE_FW) == 0)
            {
                _p_main->event_publish(event_c::CMD_FWUP_CLEAR);
                _p_main->event_publish(event_c::CMD_CLOUD_GET_FWUP);
            }
        }
        else if(strcmp(iter->key.c_str(), JSON_OBJ_KEY_CMD) == 0)
        {
        }
        else if(strcmp(iter->key.c_str(), JSON_OBJ_KEY_TP) == 0)
        {
        }
        else if(strcmp(iter->key.c_str(), JSON_OBJ_KEY_SCMD) == 0)
        {
            CMSSmartPtrT<data_string> obj(new data_string, false);
            string dest_cmd = "";
            decrypt_secure_cmd(dest_cmd, iter->value);
            obj->str_val = dest_cmd;
            _p_main->event_publish(event_c::CMD_SYSTEM_CALL, event_c::OP_NONE, obj);
        }
        else if(strcmp(iter->key.c_str(), JSON_OBJ_KEY_DESC) == 0)
        {
        }
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::response_result(string &result)
{
    int ret_val = RET_ERROR;
    if(strcmp(result.c_str(), JSON_RET_OK) == 0)
    {
        ret_val = RET_OK;
    }
    else if(strcmp(result.c_str(), JSON_RET_ERROR) == 0)
    {
    }
    else if(strcmp(result.c_str(), JSON_RET_UNAUTH) == 0)
    {
        _p_main->event_publish(event_c::CMD_CLOUD_SAUTH);
    }
    else if(strcmp(result.c_str(), JSON_RET_EXPIRED) == 0)
    {
        _p_main->event_publish(event_c::CMD_CLOUD_SAUTH);
    }
    else if(strcmp(result.c_str(), JSON_RET_NOT_FOUNT) == 0)
    {
        if(_not_found_flag == 0)
        {
            _p_main->event_publish(event_c::CMD_CLOUD_ST_CLEAR);
        }
        _not_found_flag++;
        log_i("%s not_found_flag[%d]\n", __func__, _not_found_flag);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::fwup_clear(void)
{
    int ret_val = RET_OK;
    string value = "";
    config_manager::instance()->set_cloud_fw_device(value);
    config_manager::instance()->set_cloud_fw_version(value);
    config_manager::instance()->set_cloud_fw_url(value);
    config_manager::instance()->set_cloud_fw_csurl(value);
    config_manager::instance()->set_cloud_fw_start_tm(value);
    config_manager::instance()->set_cloud_fw_span(0);
    config_manager::instance()->set_cloud_fw_status(value);
    config_manager::instance()->set_cloud_fw_reason(value);
    config_manager::instance()->set_cloud_fw_code(0);
    config_manager::instance()->set_cloud_fw_description(value);

    _p_main->kill_timer(timer::TID_CLOUD_FWUP_VERIFY);

    string rm_fw = "";
    rm_fw = "rm -rf /tmp/firmware.img";
    utils::system_call(rm_fw);
    rm_fw = "rm -rf /tmp/router_*";
    utils::system_call(rm_fw);
    rm_fw = "rm -rf /tmp/modem_*";
    utils::system_call(rm_fw);

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::fwup_prepare(result_getfwup_t result)
{
    int ret_val = RET_ERROR;

    // fwup parameter
    config_manager::instance()->set_cloud_fw_device(result.tp);
    config_manager::instance()->set_cloud_fw_version(result.up_ver);
    config_manager::instance()->set_cloud_fw_url(result.url);
    config_manager::instance()->set_cloud_fw_csurl(result.cs_url);
    config_manager::instance()->set_cloud_fw_start_tm(result.start_tm);
    config_manager::instance()->set_cloud_fw_span(result.span);

    string fw_status = JSON_FW_ST_FAILED;
    string fw_reason = JSON_FW_ERR_INTERNAL;
    int fw_code = JSON_FW_CODE_INTERNAL;
    string fw_desc = "unknown tp.";

    string dev_ver = config_manager::instance()->get_router_ver_ex();
    string mdm_ver = config_manager::instance()->get_modem_ver_ex();
    string sw_ver = "";

    if(result.tp.compare("Device") == 0)
    {
        sw_ver = dev_ver;
    }
    else if(result.tp.compare("Modem") == 0)
    {
        sw_ver = mdm_ver;
    }
    else if(result.tp.compare("Total") == 0)
    {
        sw_ver = dev_ver + "-" + mdm_ver;
    }

    // version check
    if(result.up_ver.length() > 0 && result.up_ver.compare(sw_ver) != 0)
    {
        // url check
        string dest = "";
        _auth_handle.sha256_hex_string(dest, result.url);
        if(result.url.length() > 0 && result.cs_url.compare(dest) == 0)
        {
            fw_status = JSON_FW_ST_DOWNLOADING;
            fw_reason = "";
            fw_code = 0;
            fw_desc = "OK";
            ret_val = RET_OK;
        }
        else
        {
            if(result.cs_url.compare(dest) != 0)
            {
                fw_status = JSON_FW_ST_FAILED;
                fw_reason = JSON_FW_ERR_VERIFY;
                fw_code = JSON_FW_CODE_URL;
                fw_desc = "fwup url verify failed.";
            }
            else
            {
                fw_status = JSON_FW_ST_FAILED;
                fw_reason = JSON_FW_ERR_URL;
                fw_code = JSON_FW_CODE_URL;
                fw_desc = "invalid fwup url.";
            }
        }
    }
    else
    {
        fw_status = JSON_FW_ST_FAILED;
        fw_reason = JSON_FW_ERR_VERSION;
        fw_code = JSON_FW_CODE_VERSION;
        fw_desc = "same version. this version already installed.";
    }

    config_manager::instance()->set_cloud_fw_status(fw_status);
    config_manager::instance()->set_cloud_fw_reason(fw_reason);
    config_manager::instance()->set_cloud_fw_code(fw_code);
    config_manager::instance()->set_cloud_fw_description(fw_desc);
    _p_main->event_publish(event_c::CMD_CLOUD_FWUP);

    if(ret_val == RET_OK)
    {
        _p_main->event_publish(event_c::CMD_FWUP_DOWNLOAD);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::fwup_download(void)
{
    int ret_val = RET_ERROR;
    string fw_name = "/tmp/firmware.img";
    string fw_url = config_manager::instance()->get_cloud_fw_url();
    curl_handler::response resp;

    string fw_status = "";
    string fw_reason = "";
    int fw_code = 0;
    string fw_desc = "";

    if(_curl_handle.curl_file_download(fw_url, fw_name, resp) == RET_OK)
    {
        if(resp.code == 200)
        {
            fw_status = JSON_FW_ST_DOWNLOADED;
            fw_reason = "";
            fw_code = 0;
            fw_desc = "OK";
            ret_val = RET_OK;
        }
        else
        {
            fw_status = JSON_FW_ST_FAILED;
            fw_reason = JSON_FW_ERR_FILE;
            fw_code = JSON_FW_CODE_FILE;
            fw_desc = "file access error.";
        }
    }
    else
    {
        if(resp.curl_result == 28)
        {
            // CURLE_OPERATION_TIMEDOUT (28)
            fw_status = JSON_FW_ST_FAILED;
            fw_reason = JSON_FW_ERR_TIMEOUT;
            fw_code = JSON_FW_CODE_TIMEOUT;
            fw_desc = "download timeout.";
        }
        else if(0) // todo
        {
            fw_status = JSON_FW_ST_FAILED;
            fw_reason = JSON_FW_ERR_DOWNLOAD;
            fw_code = JSON_FW_CODE_DOWNLOAD;
            fw_desc = "download error.";
        }
        else
        {
            // CURLE_COULDNT_RESOLVE_HOST (6)
            // CURLE_COULDNT_CONNECT (7)
            fw_status = JSON_FW_ST_FAILED;
            fw_reason = JSON_FW_ERR_NET;
            fw_code = JSON_FW_CODE_NET;
            fw_desc = "NetAccess failed.";
        }
    }

    config_manager::instance()->set_cloud_fw_status(fw_status);
    config_manager::instance()->set_cloud_fw_reason(fw_reason);
    config_manager::instance()->set_cloud_fw_code(fw_code);
    config_manager::instance()->set_cloud_fw_description(fw_desc);
    _p_main->event_publish(event_c::CMD_CLOUD_FWUP);

    if(ret_val == RET_OK)
    {
        _p_main->event_publish(event_c::CMD_FWUP_TIME_CALC);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::fwup_time_calculation(void)
{
    int ret_val = RET_OK;
    string start_tm = config_manager::instance()->get_cloud_fw_start_tm();
    u32 span = config_manager::instance()->get_cloud_fw_span();
    u32 msec = utils::read_random_time(start_tm, span);
    if(msec > (1000 * 60)) // 60sec
    {
        _p_main->set_timer(timer::TID_CLOUD_FWUP_VERIFY, msec, this);
    }
    else
    {
        _p_main->event_publish(event_c::CMD_FWUP_VERIFY);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::fwup_verify(void)
{
    int ret_val = RET_ERROR;
    //fw_header_t fw_header = {0,};
    //list<string> fw_list;
    //string fw_type = config_manager::instance()->get_cloud_fw_device();

    string fw_status = JSON_FW_ST_FAILED;
    string fw_reason = JSON_FW_ERR_INVALID;
    int fw_code = JSON_FW_CODE_INVALID;
    string fw_desc = "fw verify failed!!!";

    // not enough memory. when is executed fwup_install() verify and install.
    fw_status = JSON_FW_ST_INSTALLING;
    fw_reason = "";
    fw_code = 0;
    fw_desc = "OK";
    ret_val = RET_OK;
#if 0
    if(fwmgr_splite("/tmp/", "/tmp/firmware.img", &fw_header, FW_NOT_REMOVE) == 0)
    {
        log_i("fw_header[%s] cnt[%d]\n", fw_header.header_info, fw_header.file_cnt);
        for(int i = 0; i < fw_header.file_cnt; i++)
        {
            log_i("name[%s] version[%s] crc32[%d] len[%d]\n",
                                                fw_header.fw_file[i].name,
                                                fw_header.fw_file[i].version,
                                                fw_header.fw_file[i].crc32,
                                                fw_header.fw_file[i].length);

            string find_str = fw_header.fw_file[i].name;
            if(((find_str.find("router_") != string::npos) && (fw_type.compare("Device") == 0)) ||
               ((find_str.find("modem_") != string::npos) && (fw_type.compare("Modem") == 0)) )
            {
                if(fw_header.fw_file[i].length > 0)
                {
                    fw_status = JSON_FW_ST_INSTALLING;
                    fw_reason = "";
                    fw_code = 0;
                    fw_desc = "OK";
                    fw_list.push_back(fw_header.fw_file[i].name);
                    ret_val = RET_OK;
                    break;
                }
                else
                {
                    log_i("fw_name[%s] fw size is 0 !!!\n", fw_header.fw_file[i].length);
                    ret_val = RET_ERROR;
                }
            }
            else
            {
                log_i("fwup not implemented yet. fw name [%s]\n", fw_header.fw_file[i].name);
            }
        }
    }
    else
    {
        log_i("fw split failed !!!\n");
    }
#endif
    config_manager::instance()->set_cloud_fw_status(fw_status);
    config_manager::instance()->set_cloud_fw_reason(fw_reason);
    config_manager::instance()->set_cloud_fw_code(fw_code);
    config_manager::instance()->set_cloud_fw_description(fw_desc);
    _p_main->event_publish(event_c::CMD_CLOUD_FWUP);

    if(ret_val == RET_OK)
    {
        _p_main->event_publish(event_c::CMD_FWUP_INSTALL);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

int cloud_handler::fwup_install(void)
{
    int ret_val = RET_ERROR;
    log_i("%s\n", __func__);

    char err_msg[256] = {0,};
    if(fwmgr_upgrade("/tmp/firmware.img", 0, err_msg) == 0)
    {
        string reboot_reason = "FirmwareUpdate";
        config_manager::instance()->set_cloud_reboot_reason(reboot_reason);
        ret_val = RET_OK;
    }
    else
    {
        string fw_status = JSON_FW_ST_FAILED;
        string fw_reason = JSON_FW_ERR_INSTALL;
        int fw_code = JSON_FW_CODE_INSTALL;
        string fw_desc = err_msg;
        config_manager::instance()->set_cloud_fw_status(fw_status);
        config_manager::instance()->set_cloud_fw_reason(fw_reason);
        config_manager::instance()->set_cloud_fw_code(fw_code);
        config_manager::instance()->set_cloud_fw_description(fw_desc);
        _p_main->event_publish(event_c::CMD_CLOUD_FWUP);
        log_i("%s %s\n", __func__, err_msg);
    }

    if(ret_val == RET_OK)
    {
        string fw_type = config_manager::instance()->get_cloud_fw_device();
        if(fw_type.compare("Modem") == 0)
        {
            log_i("%s wait for modem update done 10sec.\n", __func__);
            usleep(1000 * 1000 * 10); // 10sec
        }
        _p_main->event_publish(event_c::CMD_REBOOT);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    return ret_val;
}

