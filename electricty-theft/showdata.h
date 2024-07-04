#ifndef SHOWDATA_H
#define SHOWDATA_H

#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include "mqtt_client.h"
#include "mysql_client.h"
#include <QLCDNumber>
#include <QTimer>
#include <QTime>


class Login;
class Showdata : public QWidget
{
    Q_OBJECT
public:
    explicit Showdata(QWidget *parent = nullptr);

private:
    QPushButton* MQTTPushButton;        //mqtt方式按钮
    QPushButton* MYSQLPushButton;       //mysql方式按钮
    QPushButton* WIFIPushButton;        //wifi连接按钮
    QPushButton* EXITPushButton;        //退出按钮
    QLCDNumber *lcdNumber;              //时间显示窗
    QTimer *timer;                      //定时器，用于更新时间

private slots:
    void MQTTevent();               //mqtt方式槽函数
    void MYSQLevent();              //mysql方式槽函数
    void WIFIevent();               //wifi连接槽函数
    void EXITevent();               //退出槽函数
    void timerTimeOut();            //定时器槽函数

signals:

};

#endif // SHOWDATA_H
