#include "common.h"
#include "log_service.h"
#include "media_task.h"

media_task::media_task(QObject *parent) :
    QObject(parent),
    _thread(),
    _mtx(),
    _resolution(100),
    _exit_flag(0)
{
    //log_d("%s", __func__);
}

media_task::~media_task()
{
    //log_d("%s", __func__);
    _exit_flag = 1;
}

void media_task::task_start()
{
    this->moveToThread(&_thread);
    connect(&_thread, SIGNAL(started()), this, SLOT(proc()));
    connect(this, SIGNAL(finished()), &_thread, SLOT(quit()));
    connect(this, SIGNAL(event_signal(event_c)), this, SLOT(event_slot(event_c)));
    _thread.start();
}

int media_task::init(main_interface *p_main, int resolution)
{
    int ret_val = RET_OK;
    log_i("media_task::%s", __func__);
    _resolution = resolution;
    _p_main = p_main;
    _p_main->event_subscribe(event_c::CMD_SELF_TEST, this);
    _p_main->event_subscribe(event_c::CMD_MEDIA_TEST, this);

    _media_handle = make_shared<QMediaPlayer>();
    _media_handle->setVolume(100);
    return ret_val;
}

int media_task::deinit(void)
{
    int ret_val = RET_OK;
    log_i("media_task::%s", __func__);

    _p_main = NULL;
    _media_handle = NULL;
    _exit_flag = 1;

    return ret_val;
}
int media_task::on_event(const event_c &ev)
{
    int ret_val = RET_OK;
    log_i("media_task::%s cmd[%d]", __func__, ev._cmd);
    //emit event_signal(ev);
    switch(ev._cmd)
    {
    case event_c::CMD_SELF_TEST:
        log_d("CMD_SELF_TEST");
        selftest();
        break;
    case event_c::CMD_MEDIA_TEST:
        log_d("CMD_MEDIA_TEST");
        break;
    default:
        break;
    }
    return ret_val;
}

void media_task::proc(void)
{
    while(_exit_flag == 0)
    {
        _mtx.lock();

        _mtx.unlock();
        QThread::msleep(_resolution);
    }

    emit finished();
    log_i("%s exit !!!", __func__);
}

void media_task::event_slot(event_c ev)
{
    log_i("media_task::%s cmd[%d]", __func__, ev._cmd);
    switch(ev._cmd)
    {
    case event_c::CMD_SELF_TEST:
        log_d("CMD_SELF_TEST");
        selftest();
        break;
    case event_c::CMD_MEDIA_TEST:
        log_d("CMD_MEDIA_TEST");
        break;
    default:
        break;
    }
}

int media_task::selftest(void)
{
    int ret_val = RET_OK;
    log_d("media_task::%s", __func__);
    _p_main->event_publish(event_c::CMD_MEDIA_TEST);
#if 1

    QMediaPlaylist *playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl::fromLocalFile("D:/source/qt_project/ev1000_client/data/resources/wav/15.wav"));
    playlist->addMedia(QUrl::fromLocalFile("D:/source/qt_project/ev1000_client/data/resources/wav/16.wav"));
    playlist->addMedia(QUrl::fromLocalFile("D:/source/qt_project/ev1000_client/data/resources/wav/17.wav"));

    playlist->setCurrentIndex(2);


    //QString path = QString("/data/resources/wav/");
    //QString wavfile
    //path.append(wavfile);
    QString path = QString("D:/source/qt_project/ev1000_client/data/resources/wav/15.wav");

    if(_media_handle->state() == QMediaPlayer::PlayingState)
       qDebug() << "still playing wav, ignored";
    else if(_media_handle->state() == QMediaPlayer::PausedState)
       qDebug() << "Pause wav, ignored";
    else
    {
        qDebug() << "media play";
       //_media_handle->setMedia(QUrl::fromLocalFile(path));
       _media_handle->setPlaylist(playlist);
       _media_handle->play();
    }
#endif

    return ret_val;
}

