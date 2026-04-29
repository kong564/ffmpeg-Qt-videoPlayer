#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>

#include "config.h"

namespace Ui {
class videoPlayer;
}

class videoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit videoPlayer(QWidget *parent = nullptr);
    ~videoPlayer();
    int getSliderValue() const;

private:
    void buildConnect();
    void setImage();
    QString formatTime(double seconds);

signals:
    void videoPlayerSignals(VIDEO_PLAYER_SIGNAL signal);

public slots:
        void onImageReceived(const QImage &image);
        void onDurationReceived(const double &duration);
        void onCurrentTimeReceived(const double &time);
        void onDecoderExit();

private:
    Ui::videoPlayer *ui;
    double m_duration;
};

#endif // VIDEOPLAYER_H
