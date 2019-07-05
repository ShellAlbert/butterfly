#include <QApplication>
#include <QDebug>
extern "C"
{
#include <gst/gst.h>
#include <glib.h>
}
#include <opencv2/opencv.hpp>
#include <video/zrtspthread.h>
#include <video/zusbthread.h>
#include <ui/zmainui.h>
int main(int argc, char *argv[])
{
    ZMainUI *win;
    ZRtspThread *rtsp[2];
    ZUSBThread *usb;

    QApplication app(argc, argv);
    rtsp[0]=new ZRtspThread("192.168.137.10");
    rtsp[1]=new ZRtspThread("192.168.137.12");
    //usb=new ZUSBThread("/dev/video1");
    win=new ZMainUI;
    if(win->ZDoInit()<0)
    {
        qDebug()<<"<error>:failed to initial main window.";
        return -1;
    }

    QObject::connect(rtsp[0],SIGNAL(ZSigNewImg(QImage)),win->ZGetDispUI(0),SLOT(ZSlotFlushImg(QImage)));
    //QObject::connect(usb,SIGNAL(ZSigNewImg(QImage)),win->ZGetDispUI(1),SLOT(ZSlotFlushImg(QImage)));
    QObject::connect(rtsp[1],SIGNAL(ZSigNewImg(QImage)),win->ZGetDispUI(2),SLOT(ZSlotFlushImg(QImage)));
    //win->show();
    //win->showMaximized();
    win->showFullScreen();
    rtsp[0]->start();
    rtsp[1]->start();
    //usb->start();
    return app.exec();
//    std::string gstreamer_pipe="rtspsrc location=\"rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=0.sdp\" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! appsink";
//    cv::VideoCapture cap(gstreamer_pipe,cv::CAP_GSTREAMER);
//    if(!cap.isOpened())
//    {
//        qDebug()<<"failed to open rtsp!";
//        return -1;
//    }
//    qDebug()<<"open rtsp okay";

//    //cv::namedWindow("mainUI",cv::WINDOW_AUTOSIZE);
//    cv::Mat img;
//    while(1)
//    {
//        if(!cap.read(img))
//        {
//            qDebug()<<"error to read img";
//            break;
//        }
//        //cv::imshow("mainUI",img);
//        qDebug()<<"read img okay"<<img.cols<<img.rows<<img.depth()<<img.channels();
//    }
//    cap.release();
//    cv::destroyAllWindows();
//    return 0;

#if 0
    GMainLoop *loop;
    GstBus *bus;
    GstElement *pipeline;
    GstElement *rtspsrc;
    GstElement *rtph264depay;
    GstElement *h264parse;
    GstElement *omxh264dec;
    GstElement *nvvidconv;
    GstElement *videoconvert;
    GstElement *ximagesink;
    GstElement *capsfilter;

    GstMessage *msg;

    //initial Gstreamer.
    gst_init(&argc,&argv);
    loop=g_main_loop_new(NULL,FALSE);

    //build the pipeline.
    //rtspsrc location="rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=0.sdp" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! ximagesink
    //pipeline=gst_parse_launch("rtspsrc location=\"rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=0.sdp\" ! rtph264depay ! h264parse ! omxh264dec ! nvvidconv ! video/x-raw,width=1920,height=1080,format=BGRx ! videoconvert ! ximagesink",NULL);
    pipeline=gst_pipeline_new("pipeline");
    rtspsrc=gst_element_factory_make("rtspsrc","rtspsrc0");
    rtph264depay=gst_element_factory_make("rtph264depay","rtph264depay0");
    h264parse=gst_element_factory_make("h264parse","h264parse0");
    omxh264dec=gst_element_factory_make("omxh264dec","omxh264dec0");
    nvvidconv=gst_element_factory_make("nvvidconv","nvvidconv0");
    capsfilter=gst_element_factory_make("capsfilter","capsfilter0");
    videoconvert=gst_element_factory_make("videoconvert","videoconvert0");
    ximagesink=gst_element_factory_make("ximagesink","ximagesink0");

    GstCaps *caps=gst_caps_new_simple("video/x-raw", \
                             "format",G_TYPE_STRING,"BGRx", \
                             "width",G_TYPE_INT,1920, \
                             "height",G_TYPE_INT,1080,NULL);
    g_object_set(G_OBJECT(capsfilter),"caps",caps,NULL);

    g_object_set(G_OBJECT(rtspsrc),"location",\
                 "rtsp://192.168.137.10:554/user=admin&password=&channel=1&stream=0.sdp",\
                 NULL);

    gst_bin_add_many(GST_BIN(pipeline),///<
                     rtspsrc,///<
                     rtph264depay,///<
                     h264parse,///<
                     omxh264dec,///<
                     nvvidconv,///<
                     capsfilter,///<
                     videoconvert,///<
                     ximagesink,NULL);
    gst_element_link_many( ///<
                           rtspsrc,///<
                           rtph264depay,///<
                           h264parse,///<
                           omxh264dec,///<
                           nvvidconv,///<
                           capsfilter,///<
                           videoconvert,///<
                           ximagesink,NULL);
    //start playing.
    gst_element_set_state(pipeline,GST_STATE_PLAYING);
    g_main_loop_run(loop);

    //wait until error or EOS.
    bus=gst_element_get_bus(pipeline);
    msg=gst_bus_timed_pop_filtered(bus,GST_CLOCK_TIME_NONE,GST_MESSAGE_ERROR);

    //free resource.
    if(msg!=NULL)
    {
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);
#endif
}
