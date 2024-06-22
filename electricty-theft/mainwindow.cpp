#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setGeometry(0, 0, 1024, 600);
    this->windows_init();
//    system("ln -sf /usr/share/zoneinfo/Asia/Hong_Kong /etc/localtime");
//    //自动对时
//    system("ntpdate time.pool.aliyun.com");
}

MainWindow::~MainWindow()
{
}

void MainWindow::windows_init()
{
    login = new Login(this);
    this->setObjectName("project");
    login->setGeometry(0,0,1024,600);
    login->show();      //显示登录界面
}
