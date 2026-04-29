#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>

class audioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit audioPlayer(QObject *parent = nullptr);

signals:

};

#endif // AUDIOPLAYER_H
