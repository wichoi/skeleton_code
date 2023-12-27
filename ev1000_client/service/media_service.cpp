#include "common.h"
#include "log_service.h"
#include "media_service.h"

media_service::media_service(QObject *parent)
    : QObject(parent)
{}

media_service::~media_service()
{
    //log_d("%s", __func__);
}

int media_service::init(void)
{
    int ret_val = RET_OK;
    log_i("media_service::%s", __func__);

    _p_handle = make_shared<QMediaPlayer>();
    player->setVolume(100);

    return ret_val;
}

int media_service::deinit(void)
{
    int ret_val = RET_OK;
    log_i("media_service::%s", __func__);
    return ret_val;
}

int media_service::run_play_wav(QString wavfile)
{
    int ret_val = RET_OK;
    log_i("media_service::%s", __func__);

    QString path = QString("/data/resources/wav/");

    path.append(wavfile);
    if(player->state() == QMediaPlayer::PlayingState)
        qDebug() << "still playing wav, ignored";
    else if(player->state() == QMediaPlayer::PausedState)
        qDebug() << "Pause wav, ignored";
    else
    {
        player->setMedia(QUrl::fromLocalFile(path));
        //player->setVolume(100);
        player->play();
    }

    return ret_val;
}
