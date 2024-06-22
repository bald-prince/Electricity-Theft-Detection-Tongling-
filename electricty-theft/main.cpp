#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("xyinput"));     //软件盘
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
