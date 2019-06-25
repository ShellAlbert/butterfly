#ifndef ZUSBTHREAD_H
#define ZUSBTHREAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h>             /* getopt_long() */

#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>          /* for videodev2.h */

#include <linux/videodev2.h>
#define CLEAR(x) memset (&(x), 0, sizeof (x))
#define ARRAY_SIZE(a)   (sizeof(a)/sizeof((a)[0]))

#include <QThread>

struct buffer {
    void *                  start;
    size_t                  length;
};
class ZUSBThread : public QThread
{
    Q_OBJECT
public:
    ZUSBThread(QString videoNodeName);
    ~ZUSBThread();
    qint32 ZDoInit();
    qint32 ZDoClean(void);
protected:
    void run();
signals:
    void ZSigNewImg(const QImage &img);
    void ZSigFinished();
private:
    int ZIoctl(int fd,unsigned long int request,void *arg);
private:
    bool m_bCleanUp;
    QString m_videoNodeName;
};

#endif // ZUSBTHREAD_H
