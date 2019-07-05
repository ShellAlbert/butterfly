#-------------------------------------------------
#
# Project created by QtCreator 2019-06-21T10:08:03
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = butterfly
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    zgblpara.cpp \
    video/zrtspthread.cpp \
    video/zusbthread.cpp \
    ui/zimgdispui.cpp  \
    ui/zmainui.cpp \
    video/zimgprocthread.cpp

HEADERS += \
    zgblpara.h \
    video/zrtspthread.h \
    video/zusbthread.h \
    ui/zimgdispui.h \
    ui/zmainui.h \
    video/zimgprocthread.h

INCLUDEPATH += /usr/include/gstreamer-1.0
INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/aarch64-linux-gnu/glib-2.0/include

INCLUDEPATH += /usr/src/tegra_multimedia_api/include
INCLUDEPATH += /usr/src/tegra_multimedia_api/include/libjpeg-8b
INCLUDEPATH += /usr/src/tegra_multimedia_api/samples/common/algorithm/cuda
INCLUDEPATH += /usr/src/tegra_multimedia_api/samples/common/algorithm/trt
INCLUDEPATH += /usr/local/cuda/include
INCLUDEPATH += /usr/include/aarch64-linux-gnu
INCLUDEPATH += /usr/include/libdrm

LIBS += -lgstreamer-1.0 -lgobject-2.0 -lglib-2.0 -pthread
LIBS += -lopencv_dnn -lopencv_ml -lopencv_objdetect -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_imgproc -lopencv_flann -lopencv_core

#for cuda part mixing programming with GPU.
CUDA_SOURCES += $$PWD/video/cuda/yuv2rgb.cu
CUDA_SDK = /usr/local/cuda
CUDA_DIR = /usr/local/cuda

INCLUDEPATH += $$CUDA_DIR/include
QMAKE_LIBDIR +=$$CUDA_DIR/lib64
CUDA_OBJECTS_DIR = $$PWD/video/cuda

CUDA_HEADERS =-Xcompiler -I"/usr/src/tegra_multimedia_api/include" \
              -Xcompiler -I"/usr/src/tegra_multimedia_api/include/libjpeg-8b" \
              -Xcompiler -I"/usr/src/tegra_multimedia_api/samples/common/algorithm/cuda" \
              -Xcompiler -I"/usr/src/tegra_multimedia_api/samples/common/algorithm/trt" \
              -Xcompiler -I"/usr/local/cuda/include" \
              -Xcompiler -I"/usr/include/aarch64-linux-gnu" \
              -Xcompiler -I"/usr/include/libdrm"

CUDA_LIBS =   -lpthread -lv4l2 -lEGL -lGLESv2 -lX11 -lnvbuf_utils -lnvjpeg -lnvosd -ldrm -lcuda -lcudart -lnvinfer -lnvparsers \
              -L"/usr/local/cuda/lib64" \
              -L"/lib/aarch64-linux-gnu" \
              -L"/usr/lib/aarch64-linux-gnu" \
              -L"/usr/lib/aarch64-linux-gnu/tegra"

LIBS += $$CUDA_LIBS


cuda.input = CUDA_SOURCES
cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}.o
cuda.commands = $$CUDA_DIR/bin/nvcc -ccbin g++ \
                        -Xcompiler --sysroot=/ -std=c++11 \
                        $$CUDA_HEADERS \
                        -gencode arch=compute_53,code=sm_53 \
                        -gencode arch=compute_62,code=sm_62 \
                        -gencode arch=compute_72,code=sm_72 \
                        -gencode arch=compute_72,code=compute_72 \
                        -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
cuda.dependency_type = TYPE_C
QMAKE_EXTRA_COMPILERS += cuda
