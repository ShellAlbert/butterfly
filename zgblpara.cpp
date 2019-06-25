#include "zgblpara.h"
#include <QDebug>
ZGblPara::ZGblPara()
{
    this->m_bGblRst2ExitFlag=false;
}
ZGblPara gGblPara;
cv::Mat QImage2cvMat(const QImage &img)
{
    cv::Mat mat;
    //qDebug()<<"format:"<<img.format();
    switch(img.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat=cv::Mat(img.height(),img.width(),CV_8UC4,(void*)img.constBits(),img.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat=cv::Mat(img.height(),img.width(),CV_8UC3,(void*)img.constBits(),img.bytesPerLine());
        cv::cvtColor(mat,mat,CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat=cv::Mat(img.height(),img.width(),CV_8UC1,(void*)img.constBits(),img.bytesPerLine());
        break;
    default:
        break;
    }
    return mat;
}
QImage cvMat2QImage(const cv::Mat &mat)
{
    //8-bit unsigned ,number of channels=1.
    if(mat.type()==CV_8UC1)
    {
        QImage img(mat.cols,mat.rows,QImage::Format_Indexed8);
        //set the color table
        //used to translate colour indexes to qRgb values.
        img.setColorCount(256);
        for(qint32 i=0;i<256;i++)
        {
            img.setColor(i,qRgb(i,i,i));
        }
        //copy input mat.
        uchar *pSrc=mat.data;
        for(qint32 row=0;row<mat.rows;row++)
        {
            uchar *pDst=img.scanLine(row);
            memcpy(pDst,pSrc,mat.cols);
            pSrc+=mat.step;
        }
        return img;
    }else if(mat.type()==CV_8UC3)
    {
        //8-bits unsigned,number of channels=3.
        const uchar *pSrc=(const uchar*)mat.data;
        QImage img(pSrc,mat.cols,mat.rows,mat.step,QImage::Format_RGB888);
        return img.rgbSwapped();
    }else if(mat.type()==CV_8UC4)
    {
        const uchar *pSrc=(const uchar*)mat.data;
        QImage img(pSrc,mat.cols,mat.rows,mat.step,QImage::Format_ARGB32);
        return img.copy();
    }else{
        qDebug()<<"<Error>:failed to convert cvMat to QImage!";
        return QImage();
    }
}
