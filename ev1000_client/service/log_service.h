#ifndef LOG_SERVICE_H
#define LOG_SERVICE_H

#include <QDebug>
#include <QTime>
#include <QDateTime>

#define log_d(fmt, ...) qDebug("[%s] "fmt, QTime::currentTime().toString("hh:mm:ss.zzz").toStdString().c_str(), ##__VA_ARGS__)
#define log_i(fmt, ...) qInfo("[%s] "fmt, QTime::currentTime().toString("hh:mm:ss.zzz").toStdString().c_str(), ##__VA_ARGS__)
#define log_w(fmt, ...) qWarning("[%s] "fmt, QTime::currentTime().toString("hh:mm:ss.zzz").toStdString().c_str(), ##__VA_ARGS__)
#define log_e(fmt, ...) qCritical("[%s] "fmt, QTime::currentTime().toString("hh:mm:ss.zzz").toStdString().c_str(), ##__VA_ARGS__)
#define log_f(fmt, ...) qFatal("[%s] "fmt, QTime::currentTime().toString("hh:mm:ss.zzz").toStdString().c_str(), ##__VA_ARGS__)

#define log_raw(fmt, ...) qInfo(fmt, ##__VA_ARGS__)

void log_file_handler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // LOG_SERVICE_H
