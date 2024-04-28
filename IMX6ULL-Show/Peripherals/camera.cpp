#include "camera.h"

Camera::Camera(QWidget *parent) : QMainWindow(parent)
{
    layoutInit();
    /* 扫描摄像头 */
    scanCameraDevice();
}

void Camera::layoutInit()
{
    //实例化与布局，常规操作
    mainWidget = new QWidget();
    photoLabel = new QLabel();
    rightWidget = new QWidget();
    comboBox = new QComboBox();
    pushButton[0] = new QPushButton();
    pushButton[1] = new QPushButton();
    scrollArea = new QScrollArea();
    displayLabel = new QLabel(scrollArea);
    vboxLayout = new QVBoxLayout();
    hboxLayout = new QHBoxLayout();

    vboxLayout->addWidget(photoLabel);
    vboxLayout->addWidget(comboBox);
    vboxLayout->addWidget(pushButton[0]);
    vboxLayout->addWidget(pushButton[1]);

    rightWidget->setLayout(vboxLayout);

    hboxLayout->addWidget(scrollArea);
    hboxLayout->addWidget(rightWidget);
    mainWidget->setLayout(hboxLayout);

    this->setCentralWidget(mainWidget);

    pushButton[0]->setMaximumHeight(40);
    pushButton[0]->setMaximumWidth(200);

    pushButton[1]->setMaximumHeight(40);
    pushButton[1]->setMaximumWidth(200);

    comboBox->setMaximumHeight(40);
    comboBox->setMaximumWidth(200);
    photoLabel->setMaximumSize(100, 75);
    scrollArea->setMinimumWidth(this->width() - comboBox->width());

    //显示图像最大画面为xx
    displayLabel->setMinimumWidth(scrollArea->width());
    displayLabel->setMinimumHeight(scrollArea->height());
    scrollArea->setWidget(displayLabel);

    //居中显示
    scrollArea->setAlignment(Qt::AlignCenter);

    //自动拉伸
    photoLabel->setScaledContents(true);
    displayLabel->setScaledContents(true);

    //设置一些属性
    pushButton[0]->setText("拍照");
    pushButton[0]->setEnabled(false);
    pushButton[1]->setText("开始");
    pushButton[1]->setCheckable(true);

    //摄像头
    camera = new Camera_hardware(this);

    //信号连接槽
    connect(camera, SIGNAL(readyImage(QImage)),this, SLOT(showImage(QImage)));
    connect(pushButton[1], SIGNAL(clicked(bool)),camera, SLOT(cameraProcess(bool)));
    connect(pushButton[1], SIGNAL(clicked(bool)),this, SLOT(setButtonText(bool)));
    connect(pushButton[0], SIGNAL(clicked()),this, SLOT(saveImageToLocal()));
}

void Camera::scanCameraDevice()
{
    //QFile文件指向/dev/video0
    QFile file("/dev/video0");
    //如果文件存在
    if (file.exists())
        comboBox->addItem("video0");
    else
    {
        displayLabel->setText("无摄像头设备");
        return;
    }

    file.setFileName("/dev/video1");

    if (file.exists())
    {
        comboBox->addItem("video1");
        //开发板ov5640等设备是1
        comboBox->setCurrentIndex(1);
    }

    file.setFileName("/dev/video2");

    if (file.exists())
        //开发板USB摄像头设备是2
        comboBox->addItem("video2");
    connect(comboBox,SIGNAL(currentIndexChanged(int)),camera, SLOT(selectCameraDevice(int)));
}

void Camera::showImage(const QImage &image)
{
    //显示图像
    displayLabel->setPixmap(QPixmap::fromImage(image));
    saveImage = image;

    //判断图像是否为空，空则设置拍照按钮不可用
    if (!saveImage.isNull())
        pushButton[0]->setEnabled(true);
    else
        pushButton[0]->setEnabled(false);
}

void Camera::setButtonText(bool bl)
{
    if (bl)
    {
        //设置摄像头设备
        camera->selectCameraDevice(comboBox->currentIndex());
        pushButton[1]->setText("关闭");
    }
    else
    {
        //若关闭了摄像头则禁用拍照按钮
        pushButton[0]->setEnabled(false);
        pushButton[1]->setText("开始");
        displayLabel->clear();
        photoLabel->clear();
    }
}

void Camera::saveImageToLocal()
{
    //判断图像是否为空
    if (!saveImage.isNull())
    {
        QString fileName = QCoreApplication::applicationDirPath() + "/test.png";
        qDebug()<<"正在保存"<<fileName<<"图片,请稍候..."<<endl;
        //save(arg1，arg2，arg3)重载函数，arg1代表路径文件名，
        //arg2保存的类型，arg3代表保存的质量等级
        saveImage.save(fileName, "PNG", -1);
        // 设置拍照的图像为显示在photoLabel上
        photoLabel->setPixmap(QPixmap::fromImage(QImage(fileName)));
        qDebug()<<"保存完成！"<<endl;
    }
}



