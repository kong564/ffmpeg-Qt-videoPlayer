#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include <QObject>

class audioDecoder : public QObject
{
    Q_OBJECT
public:
    explicit audioDecoder(QObject *parent = nullptr);

private:


signals:

};

#endif // AUDIODECODER_H
