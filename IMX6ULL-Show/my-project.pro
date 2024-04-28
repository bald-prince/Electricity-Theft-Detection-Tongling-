QT       += core gui charts network virtualkeyboard quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

#RESOURCES += qml.qrc \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(./Board/Board.pri)
INCLUDEPATH += $$PWD/Board

include(./Peripherals/Peripherals.pri)
INCLUDEPATH += $$PWD/Peripherals

#include(./Keyboard/Keyboard.pri)
#INCLUDEPATH += $$PWD/Keyboard

INCLUDEPATH += /home/ycl/opencv-3.1.0/install/include
LIBS += /home/ycl/opencv-3.1.0/install/lib/libopencv_core.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_highgui.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_imgproc.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_videoio.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_imgcodecs.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_calib3d.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_features2d.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_video.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_ml.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_objdetect.so \
        /home/ycl/opencv-3.1.0/install/lib/libopencv_flann.so

INCLUDEPATH += /home/ycl/tools/cjson/include/json-c
LIBS += /home/ycl/tools/cjson/lib/libjson-c.so \
        /home/ycl/tools/cjson/lib/libjson-c.so.5.3.0 \
        /home/ycl/tools/cjson/lib/libjson-c.a  \
        /home/ycl/tools/cjson/lib/libjson-c.so.5 \
#    /home/ycl/qt_demo/my-project/libtgtsmlInputContextPlugin.so.1.0.0


INCLUDEPATH += $$PWD/cJson/.

unix:!macx: LIBS += -L $$PWD/arm_qtmqtt/lib/ -lQt5Mqtt

INCLUDEPATH += $$PWD/arm_qtmqtt/src/mqtt/.
DEPENDPATH += /home/ycl/qt_demo/my-project/arm_qtmqtt/.


