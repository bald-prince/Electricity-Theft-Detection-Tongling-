#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include "login.h"
#include <QPainter>
#include <QPixmap>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Login *login;           //登录界面
    void windows_init();    //窗口初始化
};
#endif // MAINWINDOW_H
