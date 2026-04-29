#ifndef CONFIG_H
#define CONFIG_H

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>
}

#include <QString>

#define PHOTO_DIR "./photos/"



const int src_width = 640;
const int src_height = 480;
const AVPixelFormat src_fmt = AV_PIX_FMT_YUVJ420P;

const int des_width = 640;
const int des_height = 480;
const AVPixelFormat des_fmt = AV_PIX_FMT_YUV420P;


enum LABEL
{
  STATUS = 1,
  NETWORK,
  VIDEO_BTN_LABEL,
};

enum WINDOW_BUTTON
{
    WINDOW_BUTTON_ALBUM = 1,
    WINDOW_BUTTON_EXIT,
    WINDOW_BUTTON_STOP,
    WINDOW_BUTTON_VIDEO,
    WINDOW_BUTTON_PLAY
};

enum VIDEO_PLAYER_SIGNAL
{
    VIDEO_PLAYER_BUTTON_PAUSE = 1,
    VIDEO_PLAYER_BUTTON_FILE,
    VIDEO_PLAYER_BUTTON_EXIT,
    VIDEO_PALYER_SLIDER_REALSE,
    VIDEO_PALYER_SLIDER_PRESS
};

enum AlbumSignal
{
    ALBUM_EXIT = 1,
    ALBUM_LEFT,
    ALBUM_RIGHT,
    ALBUM_DELETE,
    ALBUM_UPLOADING
};

enum LOWER_STATUS
{
    LOWER_WIDGET = 1,
    LOWER_ALBUM
};

enum VIDEO_STATUS
{
    VIDEO_START = 1,
    VIDEO_STOP
};

enum STREAM_STATUS
{
    STREAM_ON = 1,
    STREAM_OFF
};

//struct VideoConfig
//{
//    int width;
//    int height;
//    AVPixelFormat fmt;
//    const char* filenNmae;
//};

#endif // CONFIG_H
