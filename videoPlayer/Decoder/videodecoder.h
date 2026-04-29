#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QThread>
#include <QImage>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
}

class VideoDecoder : public QThread
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);
    ~VideoDecoder();

    void startPlay(const QString &path);
    void pausePlay();
    void continuePlay();
    void stopPlay();
    void seek(int targetSecond);
    bool getIsPause();
    double getCurrentTime();
    double getTotalTime();

signals:
    void sigSendImage(const QImage &image); // 每一帧的画面
    void sigSendDuration(const double &duration);
    void sigSendFinished();

protected:
    void run() override;

private:
    bool m_isRun = false;
    bool m_isPause = false;
    bool m_isSeek = false;


    QMutex m_mutex;
    QWaitCondition m_condition;

    QTimer* timer = nullptr;
    double current_elapsed = 0;
    double duration = 0;
    int m_targetSecond = 0;
    int total_seek_duration = 0;

    double pause_start = 0;
    double total_pause_duration = 0;

    QString m_filePath;
    int m_videoDuration;
};

#endif // VIDEODECODER_H
