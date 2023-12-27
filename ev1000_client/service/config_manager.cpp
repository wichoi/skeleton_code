#include "common.h"
#include "log_service.h"
#include "config_manager.h"

config_manager* config_manager::_singleton_instance = NULL;

config_manager::config_manager()
{
    //log_d("%s", __func__);
    set_default();
}

config_manager::~config_manager()
{
    //log_d("%s", __func__);
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
    log_d("%s", __func__);

    // config info
    _config_version = "0.0.1";
    _config_time = "1970/01/01 00:00:00.000";

    // device info
    _model_name = "AT-EV1000";
    _serial = "EV00000001";
    _dev_version = "0.0.1";
    _dev_ver_ex = "0.0.1.1";

    // server url
    _server_url = "ws://222.239.231.103:9090/webServices/ocpp";
    _fwup_url = "ws://222.239.231.103:9090/webServices/ocpp";
    _file_up_url= "ws://222.239.231.103:9090/webServices/ocpp";

    // system info
    _uptime = 0;
    _timer_resolution = 10;

    return RET_OK;
}

// =============================================================
// config info
QString& config_manager::get_config_version(void)
{
    log_i("%s value[%s]", __func__, _config_version.toStdString().c_str());
    return _config_version;
}

int config_manager::set_config_version(QString &value)
{
    _config_version = value;
    log_i("%s value[%s]", __func__, _config_version.toStdString().c_str());
    return RET_OK;
}

QString& config_manager::get_config_time(void)
{
    log_i("%s value[%s]", __func__, _config_time.toStdString().c_str());
    return _config_time;
}

int config_manager::set_config_time(QString &value)
{
    _config_time = value;
    log_i("%s value[%s]", __func__, _config_time.toStdString().c_str());
    return RET_OK;
}

// =============================================================
// device info
QString& config_manager::get_model_name(void)
{
    log_i("%s value[%s]", __func__, _model_name.toStdString().c_str());
    return _model_name;
}

int config_manager::set_model_name(QString &value)
{
    _model_name = value;
    log_i("%s value[%s]", __func__, _model_name.toStdString().c_str());
    return RET_OK;
}

QString& config_manager::get_serial(void)
{
    log_i("%s value[%s]", __func__, _serial.toStdString().c_str());
    return _serial;
}

int config_manager::set_serial(QString &value)
{
    _serial = value;
    log_i("%s value[%s]", __func__, _serial.toStdString().c_str());
    return RET_OK;
}

QString& config_manager::get_dev_version(void)
{
    log_i("%s value[%s]", __func__, _dev_version.toStdString().c_str());
    return _dev_version;
}

int config_manager::set_dev_version(QString &value)
{
    _dev_version = value;
    log_i("%s value[%s]", __func__, _dev_version.toStdString().c_str());
    return RET_OK;
}

QString& config_manager::get_dev_ver_ex(void)
{
    log_i("%s value[%s]", __func__, _dev_ver_ex.toStdString().c_str());
    return _dev_ver_ex;
}

int config_manager::set_dev_ver_ex(QString &value)
{
    _dev_ver_ex = value;
    log_i("%s value[%s]", __func__, _dev_ver_ex.toStdString().c_str());
    return RET_OK;
}

// =============================================================
// server url



// =============================================================
// system info
int config_manager::get_uptime(void)
{
    log_i("%s value[%d]", __func__, _uptime);
    return _uptime;
}

int config_manager::set_uptime(int value)
{
    _uptime = value;
    log_i("%s value[%d]", __func__, _uptime);
    return RET_OK;
}

int config_manager::get_timer_resolution(void)
{
    log_i("%s value[%d]", __func__, _timer_resolution);
    return _timer_resolution;
}

int config_manager::set_timer_resolutione(int value)
{
    _timer_resolution = value;
    log_i("%s value[%d]", __func__, _timer_resolution);
    return RET_OK;
}
