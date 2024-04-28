#ifndef CANERA_H
#define CANERA_H

#include <QMainWindow>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QScreen>
#include "camera_hardware.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


using namespace cv;
class Camera_hardware;
class Camera : public QMainWindow
{
    Q_OBJECT
public:
    explicit Camera(QWidget *parent = nullptr);
private:
    //主容器，Widget也可以当作一种容器
    QWidget *mainWidget;
    //滚动区域，方便开发高分辨率
    QScrollArea *scrollArea;
    //将采集到的图像使用Widget显示
    QLabel *displayLabel;
    // 界面右侧区域布局
    QHBoxLayout *hboxLayout;
    //界面右侧区域布局
    QVBoxLayout *vboxLayout;
    //界面右侧区域容器
    QWidget *rightWidget;
    //界面右侧区域显示拍照的图片
    QLabel *photoLabel;
    //界面右侧区域摄像头设备下拉选择框
    QComboBox *comboBox;
    //两个按钮，一个为拍照按钮，另一个是开启摄像头按钮
    QPushButton *pushButton[2];
    //拍照保存的照片
    QImage saveImage;
    //摄像头设备
    Camera_hardware *camera;
    //布局初始化
    void layoutInit();
    //扫描是否存在摄像头
    void scanCameraDevice();

private slots:
    //显示图像
    void showImage(const QImage &);
    //设置按钮文本
    void setButtonText(bool);
    //保存照片到本地
    void saveImageToLocal();
};

#endif // CANERA_H
