#ifndef JSON_MANAGER_H
#define JSON_MANAGER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

class json_manager
{
public:
    json_manager();
    ~json_manager();

#if 0
public:
    int json_config_encoder(QJsonObject &output, int input1, input2);
    int json_config_decoder(QString &output, QJsonObject &output);
#endif
};

#endif // JSON_MANAGER_H
