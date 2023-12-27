#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>

#include "log.h"
#include "json-manager.h"
#include "config-manager.h"

#include "auth-handler.h"
#include "grpc-handler.h"
#include "rms-handler.h"

// config version
#define CONF_VER_MAJOR      0
#define CONF_VER_MINOR      1

// sw version
//#define SW_VER_MAJOR        0
//#define SW_VER_MINOR        0
//#define SW_VER_BUILD        1

config_manager* config_manager::_singleton_instance = NULL;

config_manager::config_manager()
{
    log_d("%s \n", __func__);
    set_default();
}

config_manager::~config_manager()
{
    log_d("%s\n", __func__);
    delete _singleton_instance;
    _singleton_instance = NULL;
}

config_manager* config_manager::instance()
{
    if(_singleton_instance == NULL)
    {
        _singleton_instance = new config_manager();
    }
    return _singleton_instance;
}

int config_manager::set_default(void)
{
    log_d("%s\n", __func__);

    // production url
    _auth_url = "https://.com";
    _token_url = "https://.com";

    return RET_OK;
}

// =============================================================
// version
int config_manager::get_fw_name(string& fw_name)
{
    fw_name = FIRMWARE_NAME;
    log_v("%s %s\n", __func__, fw_name.c_str());
    return RET_OK;
}

// =============================================================
// config version
u32 config_manager::get_con_ver_major(void)
{
    log_v("%s [%d]\n", __func__, CONF_VER_MAJOR);
    return CONF_VER_MAJOR;
}

u32 config_manager::get_con_ver_minor(void)
{
    log_v("%s [%d]\n", __func__, CONF_VER_MINOR);
    return CONF_VER_MINOR;
}

int config_manager::get_hw_version(string& ver)
{
    ver = "1.0";
    log_v("%s url[%s]\n", __func__, ver.c_str());
    return RET_OK;
}

int config_manager::get_rms_version(string& ver)
{
    ver = "1.0.9";
    log_v("%s url[%s]\n", __func__, ver.c_str());
    return RET_OK;
}


// =============================================================
// server url
string& config_manager::get_auth_url(void)
{
    log_v("%s url[%s]\n", __func__, _auth_url.c_str());
    return _auth_url;
}

int config_manager::set_auth_url(string &url)
{
    _auth_url = url;
    log_v("%s url[%s]\n", __func__, _auth_url.c_str());
    return RET_OK;
}

string& config_manager::get_token_url(void)
{
    log_v("%s url[%s]\n", __func__, _token_url.c_str());
    return _token_url;
}

int config_manager::set_token_url(string &url)
{
    _token_url = url;
    log_v("%s url[%s]\n", __func__, _token_url.c_str());
    return RET_OK;
}
