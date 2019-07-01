#include "zrtspthread.h"
#include <zgblpara.h>
extern "C"
{
#include <gst/gst.h>
#include <glib.h>
}
#include <QDebug>
ZRtspThread::ZRtspThread(QString rtspAddr)
{
    this->m_rtspAddr=rtspAddr;
}
void ZRtspThread::run()
{
    QString rtspAddr=QString("rtspsrc location=\"rtsp://%1:554/user=admin&password=&channel=1&stream=0.sdp\"").arg(this->m_rtspAddr);
    rtspAddr.append(" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! appsink");
    qDebug()<<rtspAddr;
    //std::string gstreamer_pipe="rtspsrc location=\"rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=0.sdp\" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! appsink";
    //std::string gstreamer_pipe="rtspsrc location=\"rtsp://192.168.1.89:554/user=admin&password=&channel=1&stream=0.sdp\" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! appsink";
    cv::VideoCapture cap(rtspAddr.toStdString(),cv::CAP_GSTREAMER);
    if(!cap.isOpened())
    {
        qDebug()<<"failed to open rtsp!";
        return;
    }
    qDebug()<<"open rtsp okay";

    cv::Mat mat;
    while(!gGblPara.m_bGblRst2ExitFlag)
    {
        if(!cap.read(mat))
        {
            qDebug()<<"error to read img";
            break;
        }
        qDebug()<<this->m_rtspAddr<<"read img okay"<<mat.cols<<mat.rows<<mat.depth()<<mat.channels();
        QImage img=cvMat2QImage(mat);
        emit this->ZSigNewImg(img);
    }
    cap.release();
    return;
}
