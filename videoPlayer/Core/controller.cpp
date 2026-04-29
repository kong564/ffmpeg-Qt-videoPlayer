#include "controller.h"

Controller::Controller(videoPlayer *player) : m_player(player)
{
    m_timer = new QTimer(this);

    m_videoDecoder = new VideoDecoder(this);

    buildConnect();
}

void Controller::buildConnect()
{
    connect(m_player, &videoPlayer::videoPlayerSignals, this, &Controller::handlePlayerSignals);
    connect(m_videoDecoder, &VideoDecoder::sigSendFinished, this, &Controller::hanleDecoderFinished);
    connect(m_timer, &QTimer::timeout, this, &Controller::handleTiemrSignal);

    connect(m_videoDecoder, &VideoDecoder::sigSendImage, m_player, &videoPlayer::onImageReceived);
    connect(m_videoDecoder, &VideoDecoder::sigSendDuration, m_player, &videoPlayer::onDurationReceived);

    connect(this, &Controller::upDateCurrentTime, m_player, &videoPlayer::onCurrentTimeReceived);
    connect(this, &Controller::resetPlayer, m_player, &videoPlayer::onDecoderExit);

}

void Controller::handleTiemrSignal()
{
    emit upDateCurrentTime(m_videoDecoder->getCurrentTime());
    qDebug() << "current time:" << m_videoDecoder->getCurrentTime() << "s";
}

void Controller::hanleDecoderFinished()
{
    m_timer->stop();
    emit resetPlayer();
}

void Controller::handlePlayerSignals(VIDEO_PLAYER_SIGNAL signal)
{
    switch (signal)
    {
        case VIDEO_PLAYER_BUTTON_PAUSE:
            if(!m_videoDecoder->getIsPause())
            {
                m_videoDecoder->pausePlay();
                m_timer->stop();
                qDebug() << "pausePlay";
            }
            else
            {
                m_videoDecoder->continuePlay();
                m_timer->start();
                qDebug() << "continuePlay";
            }

            break;

        case VIDEO_PLAYER_BUTTON_FILE:
            {
                QString file = QFileDialog::getOpenFileName(
                    m_player,
                    "选择视频",
                    QDir::currentPath(),
                    "Video Files (*.mp4)"
                );
                m_videoDecoder->startPlay(file);
                m_videoDecoder->getTotalTime();
                m_timer->start(500);
            }
            break;

        case VIDEO_PLAYER_BUTTON_EXIT:
            m_videoDecoder->stopPlay();
            QApplication::quit();
            qDebug() << "VIDEO_PLAYER_BUTTON_EXIT";
            break;

        case VIDEO_PALYER_SLIDER_REALSE:
            m_videoDecoder->seek(m_player->getSliderValue());
            emit upDateCurrentTime(m_player->getSliderValue());
            if(!m_videoDecoder->getIsPause())
            {
                m_timer->start();
            }
            qDebug() << "seek:" << m_player->getSliderValue() << "s";
            break;

        case VIDEO_PALYER_SLIDER_PRESS:
            m_timer->stop();
            qDebug() << "can not chang slider";
            break;


        default:
        qDebug() << "Unknown signal:" << signal;
            break;
    }
}
