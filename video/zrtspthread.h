#ifndef ZRTSPTHREAD_H
#define ZRTSPTHREAD_H

#include <QThread>
#include <QImage>
class ZRtspThread : public QThread
{
    Q_OBJECT
public:
    ZRtspThread(QString rtspAddr);

protected:
    void run();
signals:
    void ZSigNewImg(const QImage &img);

private:
    void ZProcessImg(void *yuyv);
private:
    QString m_rtspAddr;
};

#endif // ZRTSPTHREAD_H
