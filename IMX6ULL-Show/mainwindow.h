#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QWidget>
#include <QList>
#include <QString>
#include <QListWidgetItem>
#include "board.h"
#include "camera.h"
#include <QTextCodec>
#include "mqtt_client.h"
#include <QTextCodec>
#include <QTouchEvent>

struct WidgetItemData {
    QString title;

    // 使用函数指针，返回QWidget *
    typedef QWidget *(*CreateInstanceFunc)();
    CreateInstanceFunc createInstance;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QWidget *widget;
    QHBoxLayout *hBoxLayout;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    QLabel *label[3];

    void windows_init();
};
#endif // MAINWINDOW_H
