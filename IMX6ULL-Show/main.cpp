#include "mainwindow.h"
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QCoreApplication>
#include <QtVirtualKeyboard>

int main(int argc, char *argv[])
{
    //qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);
    //使用触屏的窗口激活触屏事件

    MainWindow w;
    w.show();
    return a.exec();
}
