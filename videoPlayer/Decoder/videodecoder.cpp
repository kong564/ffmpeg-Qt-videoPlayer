#include "videodecoder.h"

#include <QDebug>


VideoDecoder::VideoDecoder(QObject *parent) : QThread(parent)
{

}

VideoDecoder::~VideoDecoder()
{

}

void VideoDecoder::startPlay(const QString &path)
{
    m_filePath = path;
    m_isRun = true;
    this->start();
}

void VideoDecoder::pausePlay()
{
    QMutexLocker locker(&m_mutex);
    if(!m_isPause)
    {
        m_isPause = true;
        pause_start = av_gettime() / 1000000.0;
        qDebug() << "video pause play time:" << pause_start << "s";
    }
}

void VideoDecoder::continuePlay()
{
    QMutexLocker locker(&m_mutex);
    if(m_isPause)
    {
        m_isPause = false;
        double pauseTime = (av_gettime() / 1000000.0) - pause_start;
        total_pause_duration += pauseTime;
        m_condition.wakeAll();

        qDebug() << "this pause:" << pauseTime << "s";
        qDebug() << "video continue play total pause:" << total_pause_duration << "s";
    }

}

void VideoDecoder::stopPlay()
{
    qDebug() << "------------------stopPlay------------------" ;
    if(!m_isRun)
        return;

    {
        QMutexLocker locker(&m_mutex);
        m_isRun = false;
        m_isPause = false;
        m_condition.wakeAll();
    }

    this->wait();
}

void VideoDecoder::seek(int targetSecond)
{
   QMutexLocker locker(&m_mutex);

   m_isSeek = true;

   m_targetSecond = targetSecond;
}

bool VideoDecoder::getIsPause()
{
    return m_isPause;
}

double VideoDecoder::getCurrentTime()
{
    return  current_elapsed;
}

double VideoDecoder::getTotalTime()
{
    return duration;
}

void VideoDecoder::run()
{
    int ret;
    int video_index;
    //打开文件
    AVFormatContext* fmt_ctx = avformat_alloc_context();
    ret = avformat_open_input(&fmt_ctx, m_filePath.toStdString().c_str(), NULL, NULL);
    if(ret < 0)
    {
        qDebug() << "avformat_open_input failed";
        return ;
    }
    qDebug() << "avformat_open_input success";

    //获取流信息
    avformat_find_stream_info(fmt_ctx, NULL);

    duration = fmt_ctx->duration / (double)AV_TIME_BASE;
    m_videoDuration = duration;
    emit sigSendDuration(duration);
    //确定流video_index
    video_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if(video_index >= 0)
    {
        qDebug() << "video_index = " << video_index;
    }
    else
    {
        qDebug() << "av_find_best_stream failed";
    }

    //配置解码器上下文
    const AVCodec* dec = avcodec_find_decoder(fmt_ctx->streams[video_index]->codecpar->codec_id);
    if(!dec)
    {
        qDebug() << "avcodec_find_decoder failed";
    }
    qDebug() << "av_find_best_stream success";

    AVCodecContext* dec_ctx = avcodec_alloc_context3(dec);
    avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_index]->codecpar);

    ret = avcodec_open2(dec_ctx, dec, NULL);
    if(ret < 0)
    {
        qDebug() << "avcodec_open2 failed";
    }
    qDebug() << "avcodec_open2 success";

    //配置转换器yuv->rgb
    int width = fmt_ctx->streams[video_index]->codecpar->width;
    int height = fmt_ctx->streams[video_index]->codecpar->height;

    AVPixelFormat src_format = (AVPixelFormat)fmt_ctx->streams[video_index]->codecpar->format;
    AVPixelFormat des_format = AV_PIX_FMT_RGB24;

    SwsContext* sws = sws_getContext(width, height, src_format, width, height, des_format, SWS_BILINEAR,
                                     NULL, NULL, NULL);
    if(!sws)
    {
        qDebug() << "sws_getContext failed";
    }
    qDebug() << "sws_getContext success";

    //配置packet frame
    AVPacket* pkt = av_packet_alloc();

    AVFrame* yuv_frame = av_frame_alloc();
    AVFrame* rgb_frame = av_frame_alloc();

    rgb_frame->width = width;
    rgb_frame->height = height;
    rgb_frame->format = AV_PIX_FMT_RGB24;

    av_frame_get_buffer(rgb_frame, 0);

    double start_time = av_gettime() / 1000000.0;

    while(m_isRun && av_read_frame(fmt_ctx, pkt) >= 0)
    {
        m_mutex.lock();
        while(m_isPause)
        {
            m_condition.wait(&m_mutex);
        }
        m_mutex.unlock();

        if(m_isSeek)
        {
            qDebug() << "------seek------:" << m_targetSecond;
            int64_t targetTime = m_targetSecond / av_q2d(fmt_ctx->streams[video_index]->time_base);
            av_seek_frame(fmt_ctx, video_index, targetTime, AVSEEK_FLAG_BACKWARD);
            avcodec_flush_buffers(dec_ctx);

            start_time = av_gettime() / 1000000.0 - m_targetSecond;
            total_pause_duration = 0;

            m_isSeek = false;
            m_targetSecond = -1;
            continue;
        }
        //开始解码
        if(pkt->stream_index == video_index)
        {
            ret = avcodec_send_packet(dec_ctx, pkt);

            if(ret >= 0)
            {
                while(avcodec_receive_frame(dec_ctx, yuv_frame) >= 0)
                {
                        //转为RGB
                        sws_scale(sws, yuv_frame->data, yuv_frame->linesize, 0, rgb_frame->height, rgb_frame->data, rgb_frame->linesize);

                        //数据载入QImage
                        QImage image(rgb_frame->data[0], width, height, rgb_frame->linesize[0], QImage::Format_RGB888);

                        double pts = yuv_frame->pts * av_q2d(fmt_ctx->streams[video_index]->time_base);
                        double now = av_gettime() / 1000000.0;// double elapsed = now - start_time;

                        double elapsed;
                        {
                            QMutexLocker locker(&m_mutex);
                            elapsed = now - start_time - total_pause_duration;
                            current_elapsed = elapsed;
                        }

                        qDebug() << "pts:" << pts;
                        qDebug() << "elapsed:" << elapsed;


                        //发送QImage
                        emit sigSendImage(image.copy());

                        if (pts > elapsed)
                        {
                            int delay = (pts - elapsed) * 1000;
                            qDebug() << "delay:" << delay;
                            QThread::msleep(delay);
                        }

                    }
                }
            }

            av_frame_unref(yuv_frame);
            av_packet_unref(pkt);
    }
    qDebug() << "----------------------------------";
    qDebug() << "start clean buffer";

    avcodec_send_packet(dec_ctx, NULL);
    while(avcodec_receive_frame(dec_ctx, yuv_frame) >= 0)
    {
        //转为RGB
        sws_scale(sws, yuv_frame->data, yuv_frame->linesize, 0, rgb_frame->height, rgb_frame->data, rgb_frame->linesize);

        //数据载入QImage
        QImage image(rgb_frame->data[0], width, height, rgb_frame->linesize[0], QImage::Format_RGB888);

        double pts = yuv_frame->pts * av_q2d(fmt_ctx->streams[video_index]->time_base);
        double now = av_gettime() / 1000000.0;// double elapsed = now - start_time;

        double elapsed;
        {
            QMutexLocker locker(&m_mutex);
            elapsed = now - start_time - total_pause_duration;
            current_elapsed = elapsed;
        }

        //发送QImage
        emit sigSendImage(image.copy());

        if (pts > elapsed)
        {
            int delay = (pts - elapsed) * 1000;
            qDebug() << "delay:" << delay;
            QThread::msleep(delay);
        }
    }

    emit sigSendFinished();

    //清理各个组件
    av_frame_free(&yuv_frame);
    av_frame_free(&rgb_frame);
    av_packet_free(&pkt);
}


