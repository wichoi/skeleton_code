#include <string.h>
#include <sys/file.h>

#include <iostream>
#include <list>

#include <fw-api.h>

#include "common.h"

#include "log.h"
#include "event.h"
#include "timer.h"
#include "utils.h"
#include "config-manager.h"
#include "curl-handler.h"
#include "auth-handler.h"
#include "cli.h"

static void *pthread_cb(void *arg)
{
    cli *fp_cli = (cli*)arg;
    fp_cli->proc();
}

cli::cli() :
    _thread(),
    _p_main(),
    _cli_menu(),
    _resolution(100),
    _exit_flag(0)

{
    log_d("%s\n", __func__);
}

cli::~cli()
{
    log_d("%s\n", __func__);
    _exit_flag = 1;
}

int cli::init(main_interface *p_main, u32 resolution)
{
    int ret_val = RET_OK;
    log_d("cli::%s\n", __func__);

    _p_main = p_main;

    _cli_menu.clear();
    add_cli(&cli::help, "help", "help");
    add_cli(&cli::help, "ls", "help");
    add_cli(&cli::init, "init", "init");
    add_cli(&cli::deinit, "deinit", "deinit");

    // hello world
    add_cli(&cli::help, "=", "=============== test ===============");
    add_cli(&cli::helloworld, "hello", "<param_1> <param_2>");
    add_cli(&cli::sys_call, "system", "system call");

    // timer
    add_cli(&cli::set_timer, "set-timer", "<u32 timer id> <u32 interval ms>");
    add_cli(&cli::kill_timer, "kill-timer", "<u32 timer id> ");
    add_cli(&cli::print_timer, "print-timer", "print all activation timer");

    // config
    add_cli(&cli::conf_read, "conf-read", "p520 config read");
    add_cli(&cli::conf_write, "conf-write", "p520 config write");
    add_cli(&cli::conf_update, "conf-update", "p520 config update");

    // reboot
    add_cli(&cli::cmd_reboot, "reboot", "rebooting");
    add_cli(&cli::cmd_factory, "factory", "factory reset");
    add_cli(&cli::cmd_reset, "conf-reset", "config reset");

    // test command
    add_cli(&cli::test_event, "pub", "test event publish");
    add_cli(&cli::test_nvram_get, "nvget", "nvget key");
    add_cli(&cli::test_nvram_set, "nvset", "nvset key value");
    add_cli(&cli::test_base64, "base64", "base64 test");
    add_cli(&cli::test_crypto, "crypto", "crypto test");
    add_cli(&cli::test_hex_to_char, "hex2str", "hex to str test");
    add_cli(&cli::test_set_modem_config, "modem", "modem config test");
    add_cli(&cli::test_set_delete_token, "del-tok", "delete server token");
    add_cli(&cli::test_set_delete_key, "del-key", "delete server key");
    add_cli(&cli::test_set_server_url, "serv-url", "change server url");
    add_cli(&cli::test_set_loglevel, "log-level", "log-level -I");
    add_cli(&cli::test_ip_change, "ip-change", "ip change event");
    add_cli(&cli::test_random_time, "rand", "random time");
    add_cli(&cli::test_hostbyname, "dns", "dns amms.co.kr");
    add_cli(&cli::test_modem_sms, "sms", "modem sms test");
    add_cli(&cli::test_modem_cclk, "cclk", "modem cclk test");

    // fwup
    add_cli(&cli::fwup_download, "fwdn", "firmware download test");
    add_cli(&cli::fwup_splite, "fwsp", "firmware splite test");
    add_cli(&cli::fwup_router, "fw-router", "router firmware upgrade");
    add_cli(&cli::fwup_modem, "fw-modem", "modem firmware upgrade");
    add_cli(&cli::fwup_atcmd, "fw-atcmd", "modem firmware upgrade atcmd");

    // cloud
    add_cli(&cli::cloud_auth, "cld-auth", "cloud auth");
    add_cli(&cli::cloud_sauth, "cld-sauth", "cloud sauth");
    add_cli(&cli::cloud_boot, "cld-boot", "cloud boot");
    add_cli(&cli::cloud_event_inform, "cld-event", "cloud event");
    add_cli(&cli::cloud_report, "cld-report", "cloud report");
    add_cli(&cli::cloud_heartbeat, "cld-heart", "cloud heartbeat");
    add_cli(&cli::cloud_getkey, "cld-getkey", "cloud get key");
    add_cli(&cli::cloud_keyupdated, "cld-keyup", "cloud key updated");
    add_cli(&cli::cloud_getparam, "cld-getparam", "cloud get param");
    add_cli(&cli::cloud_paramupdated, "cld-paramup", "cloud param updated");
    add_cli(&cli::cloud_getfwup, "cld-getfw", "cloud get fw");
    add_cli(&cli::cloud_fwup, "cld-fwup", "cloud fwup");

    // at command
    add_cli(&cli::at_command, "atcmd", "at command");

    // system information
    add_cli(&cli::sys_information, "sysinfo", "system info (mem, cpu, etc...)");

    add_cli(&cli::dump_log_file, "dumplog", "log file dump");

    // exit
    add_cli(&cli::restart, "restart", "restart");
    add_cli(&cli::exit, "exit", "exit");

    _resolution = resolution;
    if(pthread_create(&_thread, NULL, pthread_cb, (void*)this) < 0)
    {
        log_e("cli thread create failed !!!\n");
    }
    pthread_detach(_thread);

    return ret_val;
}

int cli::deinit(void)
{
    int ret_val = RET_OK;
    log_d("cli::%s\n", __func__);
    _exit_flag = 1;
    _p_main = NULL;
    _cli_menu.clear();
    return ret_val;
}

int cli::proc(void)
{
    int ret_val = RET_OK;
    log_i("cli::%s\n", __func__);

    while(_exit_flag == 0)
    {
        string read_str = "";
        getline(cin, read_str);

        if(read_str.length() > 0)
        {
            list<data_c>::iterator iter;
            for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
            {
                if(strncmp(read_str.c_str(), iter->_str.c_str(), iter->_str.length()) == 0)
                {
                    log_i("%s : %s\n", __func__, iter->_str.c_str());
                    list<string> param;
                    parser(read_str, &param);
                    fp func = iter->_func;
                    static cli *cli_func = this;
                    ret_val = (cli_func->*func)(&param);
                    break;
                }
            }
        }
        usleep(_resolution * 1000);
    }

    return ret_val;
}

int cli::add_cli(fp func, string str, string help)
{
    int ret_val = RET_OK;
    log_v("%s %s\n", __func__, str.c_str());

    data_c data(func, str,  help);
    _cli_menu.push_back(data);

    return ret_val;
}

int cli::parser(string read_str, list<string> *param)
{
    int ret = RET_OK;
    char *ptr = NULL;
    char *next_ptr = NULL;
    log_d("%s : %s\n", __func__, read_str.c_str());

    ptr = strtok_r((char*)read_str.c_str(), " ", &next_ptr);
    while(ptr != NULL)
    {
        char buf[128] = {0,};
        memcpy(buf, ptr, strlen(ptr));
        //log_d("%s\n", buf);
        param->push_back(buf);
        ptr = strtok_r(NULL, " ", &next_ptr);
    }

    return ret;
}

int cli::help(list<string> *param)
{
    int ret_val = RET_OK;
    printf("===============  cli menu  ===============\n");
    list<data_c>::iterator iter;
    for(iter = _cli_menu.begin(); iter != _cli_menu.end(); ++iter)
    {
        printf("%s : %s\n", iter->_str.c_str(), iter->_help.c_str());
    }
    printf("==========================================\n");
    return ret_val;
}

int cli::init(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_INIT);
    return ret_val;
}

int cli::deinit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_DEINIT);
    return ret_val;
}

int cli::on_timer(u32 id)
{
    switch(id)
    {
    case timer::TID_HELLO_WORLD:
        log_i("%s timer id[%u]\n", __func__, id);
        break;
    default:
        log_i("%s timer id[%u]\n", __func__, id);
        break;
    }
    return RET_OK;
}

int cli::helloworld(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    CMSSmartPtrT<data_string> obj(new data_string, false);
    list<string>::iterator it;
    for(it = param->begin(); it != param->end(); ++it)
    {
        log_d("%s\n", it->c_str());
        obj->str_val.append(it->c_str());
        obj->str_val.append(" ");
    }
    _p_main->event_publish(event_c::CMD_HELLOWORLD, event_c::OP_NONE, obj);
    return ret_val;
}

int cli::sys_call(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    string cmd;
    string result;

    list<string>::iterator it = param->begin();
    ++it;
    while(it != param->end())
    {
        cmd.append(it->c_str());
        if(++it != param->end())
        {
            cmd.append(" ");
        }
    }
    utils::system_call(cmd, result);
    log_i("%s\n%s\n%s\n", __func__, cmd.c_str(), result.c_str());
    return ret_val;
}

int cli::set_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    if(param->size() == 3)
    {
        list<string>::iterator it;
        u32 t_id = 0;
        u32 t_interval = 0;
        // 0 cli menu name
        it = param->begin();

        // 1 timer id
        ++it;
        u32 id = atoi(it->c_str());

        // 2 timer interval
        ++it;
        u32 interval = atoi(it->c_str());

        _p_main->set_timer(id, interval, this);
    }
    else
    {
        log_d("%s invalid param\n", __func__);
    }
    return ret_val;
}

int cli::kill_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    if(param->size() == 2)
    {
        list<string>::iterator it;

        // 0 cli menu name
        it = param->begin();

        // 1 timer id
        ++it;
        u32 id = atoi(it->c_str());

        _p_main->kill_timer(id);
    }
    else
    {
        log_d("%s invalid param\n", __func__);
    }
    return ret_val;
}

int cli::print_timer(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_TIMER_PRINT);
     return ret_val;
}

int cli::conf_read(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_READ_CONFIG);
    return ret_val;
}

int cli::conf_write(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_WRITE_CONFIG);
    return ret_val;
}

int cli::conf_update(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return ret_val;
}

int cli::cmd_reboot(list<string> *param)
{
    int ret_val = RET_OK;
    string reboot_reason = "UserReset";
    config_manager::instance()->set_cloud_reboot_reason(reboot_reason);
    _p_main->event_publish(event_c::CMD_REBOOT);
    return ret_val;
}

int cli::cmd_factory(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_FACTORY_RESET);
    return ret_val;
}

int cli::cmd_reset(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CONFIG_RESET);
    return ret_val;
}

int cli::test_event(list<string> *param)
{
    int ret_val = RET_OK;
    if(param->size() == 2)
    {
        list<string>::iterator it;

        // 0 cli menu name
        it = param->begin();

        // 1 event id
        ++it;
        u32 id = atoi(it->c_str());

        _p_main->event_publish(id);
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }

    return ret_val;
}

int cli::test_nvram_get(list<string> *param)
{
    int ret_val = RET_OK;
    if(param->size() == 2)
    {
        list<string>::iterator it;
        string key = "";
        string value = "";

        // 0 cli menu name
        it = param->begin();

        // 1 key
        ++it;
        key = it->c_str();

        utils::nvram_get_2860(key, value);
        log_i("%s key[%s] value[%s]\n", __func__, key.c_str(), value.c_str());
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }

    return ret_val;
}

int cli::test_nvram_set(list<string> *param)
{
    int ret_val = RET_OK;
    if(param->size() == 3)
    {
        list<string>::iterator it;
        string key = "";
        string value = "";

        // 0 cli menu name
        it = param->begin();

        // 1 key
        ++it;
        key = it->c_str();

        // 2 value
        ++it;
        value = it->c_str();
        utils::nvram_set_2860(key, value);
        log_i("%s key[%s] value[%s]\n", __func__, key.c_str(), value.c_str());
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }

    return ret_val;
}

int cli::test_base64(list<string> *param)
{
    int ret_val = RET_OK;
    string src = "test base64 incoding string 123456789j0=//***&&@@@";
    string encoding = "";
    string decoding = "";
    char buf[1024] = {0,};
    int len = 0;

    utils::encode_base64(encoding, src);
    utils::decode_base64(decoding, encoding);
    utils::decode_base64(buf, &len, encoding);

    log_i("%s src       %s\n", __func__, src.c_str());
    log_i("%s encoding  %s\n", __func__, encoding.c_str());
    log_i("%s decoding  %s\n", __func__, decoding.c_str());
    log_i("%s decod[%d] %s\n", __func__, len, buf);
    return ret_val;
}

int cli::test_crypto(list<string> *param)
{
    int ret_val = RET_OK;
    string src = "test crypto 1234";
    string key = "";
    string encoding = "";
    string decoding = "";

    auth_handler test_handle;
    key = (char*)test_handle.get_factory_key();
    test_handle.ase128_encrypt(encoding, src, key);
    test_handle.ase128_decrypt(decoding, encoding, key);
    log_i("%s src       %s\n", __func__, src.c_str());
    log_i("%s key       %s\n", __func__, key.c_str());
    log_i("%s encoding  %s\n", __func__, encoding.c_str());
    log_i("%s decoding  %s\n", __func__, decoding.c_str());

    string en_dest = "";
    string de_dest = "";
    test_handle.ase128_encrypt_base64(en_dest, src, key);
    test_handle.ase128_decrypt_base64(de_dest, en_dest, key);
    log_i("%s base64_en %s\n", __func__, en_dest.c_str());
    log_i("%s base64_de %s\n", __func__, de_dest.c_str());

    return ret_val;
}

int cli::test_hex_to_char(list<string> *param)
{
    int ret_val = RET_OK;
    char *src_str = "4142433031323D3d6162634d4D";
    unsigned char dest_str[32] = {0,};
    int i = 0;
    for(i = 0; i < strlen(src_str); i+=2)
    {
        utils::hex_to_char(src_str + i, dest_str + (i/2));
    }
    log_i("%s src_str   %s\n", __func__, src_str);
    log_i("%s dest_str  %s\n", __func__, dest_str);

    string src = "4142433031323D3d6162634d4D";
    string dest = "";
    utils::hex_to_string(dest, src);
    log_i("%s src       %s\n", __func__, src.c_str());
    log_i("%s dest      %s\n", __func__, dest.c_str());

    return ret_val;
}

int cli::test_set_modem_config(list<string> *param)
{
    int ret_val = RET_OK;
    string band = "8,3,1";
    string apn = "privatelte.ktfwing.com"; // "lte.ktfwing.com";
    utils::set_modem_band(band);
    utils::set_modem_apn(apn);

    return ret_val;
}

int cli::test_set_delete_token(list<string> *param)
{
    int ret_val = RET_OK;
    string value = "";
    config_manager::instance()->set_cloud_token(value);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return ret_val;
}

int cli::test_set_delete_key(list<string> *param)
{
    int ret_val = RET_OK;
    string value = "";
    config_manager::instance()->set_cloud_private_key(value);
    _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    return ret_val;
}

int cli::test_set_server_url(list<string> *param)
{
    int ret_val = RET_OK;

    if(param->size() == 2)
    {
        list<string>::iterator it;
        u32 t_id = 0;
        u32 t_interval = 0;
        // 0 cmd
        it = param->begin();

        // 1 url
        ++it;
        string url = it->c_str();
        config_manager::instance()->set_cloud_url(url);
        log_i("%s [%s]\n", __func__, url.c_str());
        _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }

    return ret_val;
}

int cli::test_set_loglevel(list<string> *param)
{
    int ret_val = RET_OK;

    if(param->size() == 2)
    {
        list<string>::iterator it;
        u32 t_id = 0;
        u32 t_interval = 0;
        // 0 cmd
        it = param->begin();

        // 1 log level
        ++it;
        string level = it->c_str();
        config_manager::instance()->set_cloud_loglevel(level);
        log_i("%s [%s]\n", __func__, level.c_str());
        _p_main->event_publish(event_c::CMD_UPDATE_CONFIG);
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }

    return ret_val;
}

int cli::test_ip_change(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_NOTIFY_IP_CHANGE);
    return ret_val;
}

int cli::test_random_time(list<string> *param)
{
    int ret_val = RET_OK;
    string start_tm = "";
    u32 span = 0;

    start_tm = "02:00:00";
    span = 3600; // 1hour
    utils::read_random_time(start_tm, span);

    start_tm = "12:00:00";
    span = 10800; // 3hour
    utils::read_random_time(start_tm, span);

    start_tm = "22:00:00";
    span = 0;
    utils::read_random_time(start_tm, span);

    return ret_val;
}

int cli::test_hostbyname(list<string> *param)
{
    int ret_val = RET_OK;
    if(param->size() == 2)
    {
        list<string>::iterator it;
        u32 t_id = 0;
        u32 t_interval = 0;
        // 0 cmd
        it = param->begin();

        // 1 url
        ++it;
        string url = it->c_str();
        //string url = config_manager::instance()->get_cloud_url();
        list<string> ip_list;
        utils::cloud_hostbyname(url, ip_list);
        log_i("%s [%s]\n", __func__, url.c_str());

        list<string>::iterator iter = ip_list.begin();
        for(iter = ip_list.begin(); iter != ip_list.end(); ++iter)
        {
            log_i("%s [%s]\n", __func__, iter->c_str());
        }
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }
    return ret_val;
}

int cli::test_modem_sms(list<string> *param)
{
    int ret_val = RET_OK;
    string sms_body = "";
    int i = 0;
    for(i = 0; i < 20; i++)
    {
        sms_body.clear();
        utils::get_modem_sms(sms_body, i);
        if(strstr(sms_body.c_str(), "*25987309*100*") != NULL)
        {
            log_i("%s sms alive detected!!!\n", __func__);
        }
        else if(strstr(sms_body.c_str(), "*147359*330*") != NULL)
        {
            log_i("%s sms reboot detected!!!\n", __func__);
        }
        else if(strstr(sms_body.c_str(), "*147359*331*") != NULL)
        {
            log_i("%s sms reboot detected!!!\n", __func__);
        }
    }

    return ret_val;
}

int cli::test_modem_cclk(list<string> *param)
{
    int ret_val = RET_OK;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int min = 0;
    int sec = 0;
    utils::update_modem_cclk(year, month, day, hour, min, sec);
    log_i("%s [%02d-%02d-%02d %02d:%02d:%02d]\n", __func__,
                year, month, day, hour, min, sec);
    utils::update_timestamp();
    return ret_val;
}

int cli::fwup_download(list<string> *param)
{
    int ret_val = RET_OK;
    //string fw_url = config_manager::instance()->get_fwup_url();
    curl_handler curl_agent;
    string url = "";
    string fw_name ="/tmp/fw.img";

    list<string>::iterator it = param->begin();
    ++it;
    while(it != param->end())
    {
        if(it->length() > 0)
        {
            url.append(it->c_str());
        }
        it++;
    }

    if(url.empty())
    {
        url = "http://220.";
    }

    curl_handler::response resp;
    curl_agent.curl_file_download(url, fw_name, resp);
    log_i("fwup_download code[%d] size[%d]", resp.code, resp.size);
    return ret_val;
}

int cli::fwup_splite(list<string> *param)
{
    int ret_val = RET_OK;

    fw_header_t fw_header = {0,};
    if(fwmgr_splite((char*)"/tmp/", (char*)"/tmp/firmware.img", &fw_header, FW_REMOVE) == 0)
    {
        log_i("fw_header [%s] cnt[%d]\n", fw_header.header_info, fw_header.file_cnt);
        for(int i = 0; i < fw_header.file_cnt; i++)
        {
            log_i("name[%s] version[%s] crc32[%d] len[%d]\n",
                        fw_header.fw_file[i].name,
                        fw_header.fw_file[i].version,
                        fw_header.fw_file[i].crc32,
                        fw_header.fw_file[i].length);
        }
    }

    return ret_val;
}

int cli::fwup_router(list<string> *param)
{
    int ret_val = RET_OK;
    char *filename = "/tmp/firmware.img";
    int file_begin = 0;
    int file_end = 0;
    char err_msg[256] ={0,};

    routerFwUpgrade(filename, file_begin, file_end, err_msg);
    log_i("%s %s\n", __func__, err_msg);

    return ret_val;
}

int cli::fwup_modem(list<string> *param)
{
    int ret_val = RET_OK;
    char *filename = "/tmp/firmware.img";
    char err_msg[256] ={0,};
    int file_end = 0;
    FILE *fp = fopen(filename, "r");
    if(fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        file_end = ftell(fp);
        fclose(fp);
    }

    modemFwUpgrade(filename, 0, file_end, err_msg);
    log_i("%s %s\n", __func__, err_msg);

    return ret_val;
}

int cli::fwup_atcmd(list<string> *param)
{
    int ret_val = RET_OK;
#if 0
    string atcmd = "riltest \"ATTEST:";
    string result = "";
    list<string>::iterator it = param->begin();
    ++it;
    while(it != param->end())
    {
        atcmd.append(it->c_str());
        if(++it != param->end())
        {
            atcmd.append(" ");
        }
    }
    atcmd.append("\"");

    utils::system_call(atcmd, result);
    log_i("%s\n%s\n%s\n", __func__, atcmd.c_str(), result.c_str());
#endif
#if 1
    char at_resp[128] = {0,};
    char unsol_resp[128] ={0,};

    string atcmd = "";
    list<string>::iterator it = param->begin();
    ++it;
    while(it != param->end())
    {
        atcmd.append(it->c_str());
        if(++it != param->end())
        {
            atcmd.append(" ");
        }
    }

    if(atcmd.length() > 0)
    {
        ret_val = sendAtCommand((char*)atcmd.c_str(), at_resp, unsol_resp, 1);
    }

    log_i("%s ret_val[%d]\n", __func__, ret_val);
    log_i("cmd[%s]\n", atcmd.c_str());
    log_i("at_resp[%s]\n", at_resp);
    log_i("unsol_resp[%s]\n", unsol_resp);
#endif
    return ret_val;
}

int cli::cloud_auth(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_AUTH);
    return ret_val;
}

int cli::cloud_sauth(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_SAUTH);
    return ret_val;
}

int cli::cloud_boot(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_BOOT);
    return ret_val;
}

int cli::cloud_event_inform(list<string> *param)
{
    int ret_val = RET_OK;
    string timestamp = "";
    utils::read_timestamp(timestamp);
    config_manager::instance()->set_cloud_reboot_time(timestamp);
    _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_NER_ERR);
    _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_IP_CHANGE);
    _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_LAN_CHANGE);
    _p_main->event_publish(event_c::CMD_CLOUD_EVENT, event_c::OP_SMS_ALIVE);
    return ret_val;
}

int cli::cloud_report(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_REPORT);
    return ret_val;
}

int cli::cloud_heartbeat(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_HEARTBEAT);
    return ret_val;
}

int cli::cloud_getkey(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_GET_KEY);
    return ret_val;
}

int cli::cloud_keyupdated(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_KEY_UPDATED);
    return ret_val;
}

int cli::cloud_getparam(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_GET_PARAM);
    return ret_val;
}

int cli::cloud_paramupdated(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_PARAM_UPDATED);
    return ret_val;
}

int cli::cloud_getfwup(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_GET_FWUP);
    return ret_val;
}

int cli::cloud_fwup(list<string> *param)
{
    int ret_val = RET_OK;
    _p_main->event_publish(event_c::CMD_CLOUD_FWUP);
    return ret_val;
}

int cli::at_command(list<string> *param)
{
    int ret_val = RET_OK;
    if(param->size() > 1)
    {
        string atcmd = "";
        string result = "";
        list<string>::iterator it = param->begin();
        ++it;
        while(it != param->end())
        {
            atcmd.append(it->c_str());
            if(++it != param->end())
            {
                atcmd.append(" ");
            }
        }
        utils::at_command(atcmd, result);
        log_i("%s cmd[%s]\n%s\n", __func__, atcmd.c_str(), result.c_str());
    }
    else
    {
        log_i("%s invalid param\n", __func__);
    }

    return ret_val;
}

int cli::sys_information(list<string> *param)
{
    int ret_val = RET_OK;
    string mem("");
    string cpu("");
    string ipaddr("");
    utils::read_mem_usage(mem);
    utils::read_cpu_usage(cpu);
    utils::read_ip_addr(ipaddr);
    log_i("mem[%s] cpu[%s] ip[%s]\n", mem.c_str(), cpu.c_str(), ipaddr.c_str());
    return ret_val;
}

int cli::dump_log_file(list<string> *param)
{
    int ret_val = RET_OK;
    string str_dump  = "";
    utils::dump_log_file(str_dump);
    log_i("%s", str_dump.c_str());
    return ret_val;
}

int cli::restart(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_RESTART);
    return ret_val;
}

int cli::exit(list<string> *param)
{
    int ret_val = RET_OK;
    log_d("%s\n", __func__);
    _p_main->event_publish(event_c::CMD_EXIT);
    return ret_val;
}

