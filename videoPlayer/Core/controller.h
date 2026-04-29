#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QApplication>
#include <QDateTime>
#include <QFileDialog>

#include "videoplayer.h"
#include "videodecoder.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(videoPlayer* player);

private:
    void buildConnect();

signals:
    void resetPlayer();
    void upDateCurrentTime(const double& time);

private slots:
    void hanleDecoderFinished();
    void handleTiemrSignal();
    void handlePlayerSignals(VIDEO_PLAYER_SIGNAL signal);

private:
    videoPlayer* m_player = nullptr;
    VideoDecoder* m_videoDecoder = nullptr;
    QTimer* m_timer = nullptr;

signals:

};

#endif // CONTROLLER_H
