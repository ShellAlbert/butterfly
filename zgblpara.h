#ifndef ZGBLPARA_H
#define ZGBLPARA_H

#include <QObject>
#include <QWidget>

class ZGblPara
{
public:
    ZGblPara();
public:
    //global request to exit flag.
    bool m_bGblRst2ExitFlag;

    //counter for captured image.
    qint64 m_nCntLft;
    qint64 m_nCntRht;

private:
};
extern ZGblPara gGblPara;

#include <opencv2/opencv.hpp>
#include <QImage>
extern cv::Mat QImage2cvMat(const QImage &img);
extern QImage cvMat2QImage(const cv::Mat &mat);
#endif // ZGBLPARA_H
