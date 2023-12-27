#ifndef MEDIA_TASK_H
#define MEDIA_TASK_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "main_interface.h"
#include "event_service.h"

class media_task :
    public QObject,
    public event_listener
{
    Q_OBJECT
public:
    explicit media_task(QObject *parent = nullptr);
    virtual ~media_task(void);
    void task_start(void);

public:
    int init(main_interface *p_main, int resolution = 100);
    int deinit(void);

    // event listener
    int on_event(const event_c &ev);

signals:
    void finished(void);
    void event_signal(event_c ev);

public slots:
    void proc(void);
    void event_slot(event_c ev);

private:
    int selftest(void);

private:
    QThread _thread;
    QMutex _mtx;
    main_interface *_p_main;
    u32 _resolution;
    int _exit_flag;
    shared_ptr<QMediaPlayer> _media_handle;
};

#endif // MEDIA_TASK_H
