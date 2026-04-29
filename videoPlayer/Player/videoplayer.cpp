#include "videoplayer.h"
#include "ui_videoplayer.h"
#include "config.h"

#include <QDebug>
#include <QPainter>

videoPlayer::videoPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::videoPlayer)
{
    ui->setupUi(this);
    buildConnect();
    setImage();
}

videoPlayer::~videoPlayer()
{
    delete ui;
}

int videoPlayer::getSliderValue() const
{
    return ui->progressSlider->value();
}

void videoPlayer::buildConnect()
{
    QVector<QPair<QString, VIDEO_PLAYER_SIGNAL>> buttonSignals = {
            {"pauseBtn", VIDEO_PLAYER_BUTTON_PAUSE},
            {"fileBtn", VIDEO_PLAYER_BUTTON_FILE},
            {"exitBtn", VIDEO_PLAYER_BUTTON_EXIT}

        };

        for (const auto& pair : buttonSignals) {
            QPushButton* button = findChild<QPushButton*>(pair.first);
            if (button) {
                connect(button, &QPushButton::clicked, this, [=]() {
                    emit videoPlayerSignals(pair.second);  // 发射窗口按钮信号
                });
            } else {
                qDebug() << "Button not found:" << pair.first;
            }
        }

        connect(ui->progressSlider, &QSlider::sliderReleased, [=](){
            emit videoPlayerSignals(VIDEO_PALYER_SLIDER_REALSE);
        });

        connect(ui->progressSlider, &QSlider::sliderPressed, [=](){
            emit videoPlayerSignals(VIDEO_PALYER_SLIDER_PRESS);
        });
}

void videoPlayer::setImage()
{
    ui->pauseBtn->setCheckable(true);

    ui->fileBtn->setStyleSheet("QPushButton{border-image:url(:/image/button/image/open_normal.png);}"
    );
    ui->pauseBtn->setStyleSheet("QPushButton{border-image:url(:/image/button/image/pause1_normal.png);}"
                                "QPushButton:checked{border-image:url(:/image/button/image/start1_normal.png);}"
    );
    ui->exitBtn->setStyleSheet("QPushButton{border-image:url(:/image/button/image/stop1_normal.png);}"
    );
}

QString videoPlayer::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);

    int hours   = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int secs    = totalSeconds % 60;

    if (hours > 0)
    {
        return QString("%1:%2:%3")
                .arg(hours,   2, 10, QChar('0'))
                .arg(minutes, 2, 10, QChar('0'))
                .arg(secs,    2, 10, QChar('0'));
    }
    else
    {
        return QString("%1:%2")
                .arg(minutes, 2, 10, QChar('0'))
                .arg(secs,    2, 10, QChar('0'));
    }
}

void videoPlayer::onImageReceived(const QImage &image)
{
    if (image.isNull()) return;

    int labelW = ui->displayLab->width();
    int labelH = ui->displayLab->height();

    QPixmap pix = QPixmap::fromImage(image).scaled(
        labelW,
        labelH,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    QPixmap canvas(labelW, labelH);
    canvas.fill(Qt::black);

    QPainter painter(&canvas);
    int x = (labelW - pix.width()) / 2;
    int y = (labelH - pix.height()) / 2;

    painter.drawPixmap(x, y, pix);

    ui->displayLab->setPixmap(canvas);
}
void videoPlayer::onDurationReceived(const double &duration)
{
       m_duration = duration;
       QString totalStr   = formatTime(duration);
       QString currentStr = formatTime(0);

       ui->progressLab->setText(currentStr + " / " + totalStr);
       ui->progressSlider->setRange(0, duration);
}

void videoPlayer::onCurrentTimeReceived(const double &time)
{
    QString totalStr   = formatTime(m_duration);
    QString currentStr = formatTime(time);

    ui->progressLab->setText(currentStr + " / " + totalStr);
    ui->progressSlider->setValue(time);
}

void videoPlayer::onDecoderExit()
{
    onDurationReceived(0);
    ui->progressSlider->setValue(0);
    ui->displayLab->setPixmap(QPixmap::fromImage(QImage()));
    ui->displayLab->setText("视频播放完成，请选择播放文件");
}
