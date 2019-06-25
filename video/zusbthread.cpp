#include "zusbthread.h"
#include "zgblpara.h"
#include <QDebug>

#include <cuda_runtime.h>
#include "video/cuda/yuv2rgb.cuh"
#include <sys/mman.h>

ZUSBThread::ZUSBThread(QString videoNodeName)
{
    this->m_videoNodeName=videoNodeName;
    this->m_bCleanUp=false;
}
ZUSBThread::~ZUSBThread()
{

}
qint32 ZUSBThread::ZDoInit()
{
    return 0;
}
qint32 ZUSBThread::ZDoClean(void)
{
    qDebug()<<"<USBCAP>:thread done.";
    //set global request exit flag to notify other threads.
    //gGblPara.m_bGblRst2ExitFlag=true;
    this->m_bCleanUp=true;
    emit this->ZSigFinished();
    return 0;
}
int ZUSBThread::ZIoctl(int fd,unsigned long int request,void *arg)
{
    int ret;
    do{
        ret=ioctl(fd,request,arg);
    }while (-1==ret && EINTR==errno);
    return ret;
}
void ZUSBThread::run()
{
    unsigned char *cuda_out_buffer=NULL;
    bool cuda_zero_copy=true;

    //1.open device.
    int fd=open("/dev/video1",O_RDWR|O_NONBLOCK,0);
    if(fd<0)
    {
        qDebug()<<"<error>:failed to open video node"<<this->m_videoNodeName;
        return;
    }
    //2.init device.
    struct v4l2_capability cap;
    if(ioctl(fd,VIDIOC_QUERYCAP,&cap)<0)
    {
        qDebug()<<"<error>:failed to query capability"<<this->m_videoNodeName;
        this->ZDoClean();
        return;
    }
    qDebug()<<"Driver Name:"<<(char*)cap.driver;
    qDebug()<<"Card Name:"<<(char*)cap.card;
    qDebug()<<"Bus Info:"<<(char*)cap.bus_info;

    if(!(cap.capabilities&V4L2_CAP_VIDEO_CAPTURE))
    {
        qDebug()<<"<error>:node doesnot support video capture"<<this->m_videoNodeName;
        this->ZDoClean();
        return;
    }

    if(!(cap.capabilities&V4L2_CAP_STREAMING))
    {
        qDebug()<<"<error>:node doesnot support streaming capture"<<this->m_videoNodeName;
        this->ZDoClean();
        return;
    }

    //enumrate all supported format.
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmtdesc.index=0;
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
    {
        qDebug()<<(char*)fmtdesc.description;
        fmtdesc.index++;
    }

    //get default format.
    struct v4l2_format fmt;
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd,VIDIOC_G_FMT,&fmt)<0)
    {
        qDebug()<<"failed to get default format.";
        return;
    }

    switch(fmt.fmt.pix.pixelformat)
    {
    case V4L2_PIX_FMT_YUYV:
        qDebug()<<"V4L2_PIX_FMT_YUYV";
        break;
    case V4L2_PIX_FMT_MJPEG:
        qDebug()<<"V4L2_PIX_FMT_MJPEG";
        break;
    default:
        qDebug()<<"other format:"<<fmt.fmt.pix.pixelformat;
        break;
    }
    qDebug("width:%d,height:%d,size=%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height,fmt.fmt.pix.sizeimage);

    //set predefined format.
    CLEAR(fmt);
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat=V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field=V4L2_FIELD_INTERLACED;
    fmt.fmt.pix.width=1280;
    fmt.fmt.pix.height=720;
    if(ioctl(fd,VIDIOC_S_FMT,&fmt)<0)
    {
        qDebug()<<"failed to set format.";
        return;
    }
    //read back to check if settings was successful.
    CLEAR(fmt);
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd,VIDIOC_G_FMT,&fmt)<0)
    {
        qDebug()<<"failed to get default format.";
        return;
    }
    switch(fmt.fmt.pix.pixelformat)
    {
    case V4L2_PIX_FMT_YUYV:
        qDebug()<<"V4L2_PIX_FMT_YUYV";
        break;
    case V4L2_PIX_FMT_MJPEG:
        qDebug()<<"V4L2_PIX_FMT_MJPEG";
        break;
    default:
        qDebug()<<"other format:"<<fmt.fmt.pix.pixelformat;
        break;
    }
    qDebug("width:%d,height:%d,size=%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height,fmt.fmt.pix.sizeimage);

    //read default fps.
    struct v4l2_streamparm param;
    CLEAR(param);
    param.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(fd,VIDIOC_G_PARM,&param)<0)
    {
        qDebug()<<"<error>:failed to VIDIOC_G_PARAM.";
        return;
    }
    qDebug("fps:%d/%d\n",param.parm.output.timeperframe.numerator,param.parm.output.timeperframe.denominator);

    /* Select video input, video standard and tune here. */
    //    struct v4l2_cropcap cropcap;
    //    CLEAR(cropcap);
    //    cropcap.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //    if(0==this->ZIoctl(fd,VIDIOC_CROPCAP,&cropcap))
    //    {
    //        struct v4l2_crop crop;
    //        crop.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //        crop.c=cropcap.defrect; /* reset to default */
    //        if(this->ZIoctl(fd,VIDIOC_S_CROP,&crop)<0)
    //        {
    //            switch (errno)
    //            {
    //            case EINVAL:
    //                /* Cropping not supported. */
    //                break;
    //            default:
    //                /* Errors ignored. */
    //                break;
    //            }
    //        }
    //    }else{
    //        /* Errors ignored. */
    //        qDebug()<<"<error>:happened in VIDIOC_CROPCAP"<<this->m_videoNodeName;
    //    }


    /* Buggy driver paranoia. */
    unsigned int min;
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
        fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
        fmt.fmt.pix.sizeimage = min;

    //IO_METHOD_MMAP
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count=4;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_MMAP;
    if(this->ZIoctl(fd,VIDIOC_REQBUFS,&req)<0)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"does not support user pointer I/O.";
        this->ZDoClean();
        return;
    }
    if(req.count<2)
    {
        qDebug()<<"Insufficient buffer memory!";
        this->ZDoClean();
        return;
    }
    struct buffer *buffers;
    buffers=static_cast<struct buffer *>(calloc(req.count,sizeof(*buffers)));
    if(!buffers)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"out of memory!";
        this->ZDoClean();
        return;
    }
    for(unsigned int i=0;i<req.count;++i)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory=V4L2_MEMORY_MMAP;
        buf.index=i;

        if(this->ZIoctl(fd,VIDIOC_QUERYBUF,&buf)<0)
        {
            qDebug()<<"<error>:failed to VIDIOC_QUERYBUF!";
            this->ZDoClean();
            return;
        }

        buffers[i].length=buf.length;
        buffers[i].start=mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,buf.m.offset);
        if(MAP_FAILED==buffers[i].start)
        {
            qDebug()<<"<error>:failed to mmap!";
            this->ZDoClean();
            return;
        }
    }

#if 0
    //IO_METHOD_USERPTR
    int buffer_size=static_cast<int>(fmt.fmt.pix.sizeimage);
    int page_size=getpagesize();
    buffer_size=(buffer_size+page_size-1)&~(page_size-1);

    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count=4;
    req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory=V4L2_MEMORY_USERPTR;
    if(this->ZIoctl(fd,VIDIOC_REQBUFS,&req)<0)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"does not support user pointer I/O.";
        this->ZDoClean();
        return;
    }

    struct buffer *buffers;
    buffers=static_cast<struct buffer *>(calloc(4,sizeof(*buffers)));
    if(!buffers)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"out of memory!";
        this->ZDoClean();
        return;
    }

    for(unsigned int i=0;i<4;++i)
    {
        buffers[i].length=static_cast<unsigned long>(buffer_size);
        if(cuda_zero_copy)
        {
            cudaMallocManaged (&buffers[i].start,buffer_size,cudaMemAttachGlobal);
        }else{
            buffers[i].start=memalign(/*boundary*/page_size,buffer_size);
        }

        if(!buffers[i].start)
        {
            qDebug()<<"<error>:"<<this->m_videoNodeName<<"out of memory!";
            this->ZDoClean();
            return;
        }
    }
#endif

    //3.init cuda.
    /* Check unified memory support. */
    if(cuda_zero_copy)
    {
        cudaDeviceProp devProp;
        cudaGetDeviceProperties (&devProp,0);
        if(!devProp.managedMemory)
        {
            qDebug()<<"CUDA device does not support managed memory.";
            cuda_zero_copy=false;
        }
    }

    /* Allocate output buffer. */
    size_t size=1280*720* 3;
    if (cuda_zero_copy){
        cudaMallocManaged(&cuda_out_buffer,size,cudaMemAttachGlobal);
    }else{
        cuda_out_buffer=(unsigned char*)malloc(size);
    }

    cudaDeviceSynchronize();


    //4.start capture.
    //IO_METHOD_MMAP.
    for(unsigned int i=0;i<4;++i)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory=V4L2_MEMORY_MMAP;
        buf.index=i;
        if(this->ZIoctl(fd,VIDIOC_QBUF,&buf)<0)
        {
            qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_QBUF!";
            this->ZDoClean();
            return;
        }
    }

    enum v4l2_buf_type type;
    type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(this->ZIoctl(fd,VIDIOC_STREAMON,&type)<0)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_STREAMON!";
        this->ZDoClean();
        return;
    }
#if 0
    //IO_METHOD_USERPTR
    for(unsigned int i=0;i<4;++i)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);
        buf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory=V4L2_MEMORY_USERPTR;
        buf.index=i;
        buf.m.userptr=reinterpret_cast<unsigned long>(buffers[i].start);
        buf.length=static_cast<__u32>(buffers[i].length);
        if(this->ZIoctl(fd,VIDIOC_QBUF,&buf)<0)
        {
            qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_QBUF!";
            this->ZDoClean();
            return;
        }
    }

    enum v4l2_buf_type type;
    type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(this->ZIoctl(fd,VIDIOC_STREAMON,&type)<0)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_STREAMON!";
        this->ZDoClean();
        return;
    }
#endif

    //5.main loop.
    while(!gGblPara.m_bGblRst2ExitFlag)
    {
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd,&fds);

        /* Timeout. */
        tv.tv_sec=2;
        tv.tv_usec=0;
        ret=select(fd+1,&fds,NULL,NULL,&tv);
        if(ret<0)
        {
            qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to select()!";
            break;
        }else if(ret==0)
        {
            //select timeout.
            continue;
        }else{
            struct v4l2_buffer getBuf;
            CLEAR(getBuf);
            getBuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
            getBuf.memory=V4L2_MEMORY_MMAP;
            if(this->ZIoctl(fd,VIDIOC_DQBUF,&getBuf)<0)
            {
                qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_DQBUF!";
            }
            //            unsigned int i;
            //            for(i=0;i<n_buffers;++i)
            //                if (buf.m.userptr == (unsigned long) buffers[i].start && buf.length == buffers[i].length)
            //                    break;

            //            assert (i<n_buffers);

            //process_image((void*)buf.m.userptr);

            gpuConvertYUYVtoRGB((unsigned char*)buffers[getBuf.index].start,cuda_out_buffer,1280,720);
            QImage img((uchar*)cuda_out_buffer,1280,720,1280*3,QImage::Format_RGB888);
            emit this->ZSigNewImg(img);
            if(this->ZIoctl(fd,VIDIOC_QBUF,&getBuf)<0)
            {
                qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_QBUF!";
            }
            //qDebug()<<"captured one frame.";
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    //6.stop capture.
    type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(this->ZIoctl(fd,VIDIOC_STREAMOFF,&type)<0)
    {
        qDebug()<<"<error>:"<<this->m_videoNodeName<<"failed to VIDIOC_STREAMOFF!";
    }
    //IO_METHOD_MMAP.
    for(unsigned int i=0;i<4;++i)
    {
        munmap(buffers[i].start,buffers[i].length);
    }
    free(buffers);
    close(fd);
    this->ZDoClean();
    return;
}
