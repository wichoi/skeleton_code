#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <QString>

class config_manager
{
private:
    config_manager();
    ~config_manager();

public:
    static config_manager* instance(void);
    int set_default(void);

// =============================================================
// config info
    QString& get_config_version(void);
    int set_config_version(QString &value);

    QString& get_config_time(void);
    int set_config_time(QString &value);

// =============================================================
// device info
    QString& get_model_name(void);
    int set_model_name(QString &value);

    QString& get_serial(void);
    int set_serial(QString &value);

    QString& get_dev_version(void);
    int set_dev_version(QString &value);

    QString& get_dev_ver_ex(void);
    int set_dev_ver_ex(QString &value);

// =============================================================
// server url



// =============================================================
// system info
    int get_uptime(void);
    int set_uptime(int value);

    int get_timer_resolution(void);
    int set_timer_resolutione(int value);

private:
    static config_manager *_singleton_instance;

    // config info
    QString _config_version;
    QString _config_time;

    // device info
    QString _model_name;
    QString _serial;
    QString _dev_version;
    QString _dev_ver_ex;

    // server url
    QString _server_url;
    QString _fwup_url;
    QString _file_up_url;

    // system info
    int _uptime;
    int _timer_resolution;
};

#endif // CONFIG_MANAGER_H
