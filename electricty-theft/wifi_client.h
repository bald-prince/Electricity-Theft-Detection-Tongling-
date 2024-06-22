#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <QMainWindow>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QLocale>
#include <QTextStream>
#include <QThread>
#include <QLabel>
#include <QListWidget>
#include <QRegularExpression>
#include <QString>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QProcess>


class Wifi_client : public QMainWindow
{
    Q_OBJECT
public:
    explicit Wifi_client(QWidget *parent = nullptr);
    bool checkConnection();
private:
    QRadioButton *RadioButtonWifi;      //wifi开关按钮
    QPushButton *BtnExit;               //wifi退出按钮
    QPushButton *BtnRefresh;            //wifi刷新按钮
    QLabel* TitleLabel;                 //wifi标题
    QListWidget *listWidget;            //wifi显示列表
    QString wifiInfoContent;
    QInputDialog* inputDialog;          //密码输入框
    QProcess *ConnetWifi = nullptr;     //wifi连接进程
    QLabel* wifiState;                  //wifi状态显示

private slots:
    void OpenWifi(bool flag);           //wifi开关槽函数
    void ExitWifi();                    //退出wifi槽函数
    void RefreshWifi();                 //刷新列表槽函数
    void onItemClicked(QListWidgetItem *item);      //点击wifi槽函数
    void connectToWifi(const QString &essid, const QString &password);      //连接wifi槽函数

signals:
};

#endif // WIFI_CLIENT_H
