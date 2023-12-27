#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>

#include "common.h"
#include "log_service.h"
#include "config_manager.h"
#include "config_service.h"

#define CONFIG_FILE                 "./ev1000_client.conf"
#define CONFIG_FILE_BAK             "./ev1000_client.conf.bak"

// config info
#define KEY_CONF_VERSION            "config_version"
#define KEY_CONF_TIME               "config_time"

// device info
#define KEY_MODEL_NAME              "model_name"
#define KEY_SERIAL                  "serial"
#define KEY_DEV_VERSION             "dev_version"
#define KEY_DEV_VER_EX              "dev_ver_ex"

// server url
#define KEY_SERVER_URL              "server_url"
#define KEY_FWUP_URL                "fwup_url"
#define KEY_FILE_URL                "file_up_url"

config_service::config_service()
{

}

config_service::~config_service()
{

}

int config_service::init()
{
    config_read();
    return RET_OK;
}

int config_service::deinit()
{
    config_write();
    return RET_OK;
}

int config_service::config_read()
{
    log_i("%s", __func__);
    int ret_val = RET_ERROR;

    if(QFile::exists(CONFIG_FILE) == true)
    {
        QFile loadFile(QStringLiteral(CONFIG_FILE));
        if(loadFile.open(QIODevice::ReadWrite))
        {
            QString value;
            QByteArray loadData = loadFile.readAll();
            QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));
            QJsonObject jsonObj = loadDoc.object();

            QString str_data(loadData);
            log_raw("%s", str_data.toStdString().c_str());

            if(jsonObj[KEY_CONF_VERSION].isString() &&
                jsonObj[KEY_CONF_VERSION].toString().length() > 0)
            {
                value = jsonObj[KEY_CONF_VERSION].toString();
                config_manager::instance()->set_config_version(value);
            }

            if(jsonObj[KEY_CONF_TIME].isString() &&
                jsonObj[KEY_CONF_TIME].toString().length() > 0)
            {
                value = jsonObj[KEY_CONF_TIME].toString();
                config_manager::instance()->set_config_time(value);
            }

            if(jsonObj[KEY_MODEL_NAME].isString() &&
                jsonObj[KEY_MODEL_NAME].toString().length() > 0)
            {
                value = jsonObj[KEY_MODEL_NAME].toString();
                config_manager::instance()->set_model_name(value);
            }

            if(jsonObj[KEY_SERIAL].isString() &&
                jsonObj[KEY_SERIAL].toString().length() > 0)
            {
                value = jsonObj[KEY_SERIAL].toString();
                config_manager::instance()->set_serial(value);
            }
            loadFile.close();
            ret_val = RET_OK;
        }
    }

    return ret_val;
}

int config_service::config_write()
{
    log_i("%s", __func__);
    int ret_val = RET_OK;
    QJsonObject jsonObj;
    jsonObj[KEY_CONF_VERSION] = config_manager::instance()->get_config_version();
    jsonObj[KEY_CONF_TIME] = config_manager::instance()->get_config_time();
    jsonObj[KEY_MODEL_NAME] = config_manager::instance()->get_model_name();
    jsonObj[KEY_SERIAL] = config_manager::instance()->get_serial();

    if(QFile::exists(CONFIG_FILE) == true)
    {
        if(QFile::exists(CONFIG_FILE_BAK) == true)
        {
            QFile::remove(CONFIG_FILE_BAK);
        }

        QFile::copy(CONFIG_FILE, CONFIG_FILE_BAK);
    }

    QFile saveFile(QStringLiteral(CONFIG_FILE));
    if(saveFile.open(QIODevice::ReadWrite))
    {
        QJsonDocument saveDoc(jsonObj);
        saveFile.write(saveDoc.toJson());
        saveFile.close();
    }
    else
    {
        log_i("Cound not open json file to save");
        ret_val = RET_ERROR;
    }

    return ret_val;
}
