#ifndef MEDIA_SERVICE_H
#define MEDIA_SERVICE_H

#include <QObject>
#include <QMediaPlayer>

#include "common.h"

class media_service : public QObject
{
    Q_OBJECT

public:
    explicit media_service(QObject *parent = nullptr);
    ~media_service();

public:
    int init(void);
    int deinit(void);

private:
    shared_ptr<QMediaPlayer> _p_handle;
};

#endif // MEDIA_SERVICE_H
