#include "showdata.h"
#include "login.h"
#include "wifi_client.h"

Showdata::Showdata(QWidget *parent) : QWidget(parent)
{
    this->setGeometry(0, 0, 1024, 600);
    QPixmap backgroundPixmap(":/icons/desk.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, backgroundPixmap);
    this->setPalette(palette);
    this->setAutoFillBackground(true); // 确保窗口自动填充背景

    MQTTPushButton = new QPushButton(this);//mqtt按键
    MYSQLPushButton = new QPushButton(this);//mysql按键
    EXITPushButton = new QPushButton(this);//退出按键
    WIFIPushButton = new QPushButton(this);//退出按键

    MQTTPushButton->setGeometry(40,40,100,100);
    MYSQLPushButton->setGeometry(160,40,100,100);
    WIFIPushButton->setGeometry(280,40,100,100);
    EXITPushButton->setGeometry(900,250,100,100);

    MQTTPushButton->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/MQTT.png);"
                "background-position: center;}"  // 图片居中
                 );

    MYSQLPushButton->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/mysql.png);"
                "background-position: center;}"  // 图片居中
                 );

    WIFIPushButton->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/wifi.png);"
                "background-position: center;}"  // 图片居中
                 );

    EXITPushButton->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/exit.png);"
                "background-position: center;}"  // 图片居中
                 );
    connect(MQTTPushButton, SIGNAL(clicked()), this, SLOT(MQTTevent()));
    connect(MYSQLPushButton, SIGNAL(clicked()), this, SLOT(MYSQLevent()));
    connect(WIFIPushButton, SIGNAL(clicked()), this, SLOT(WIFIevent()));
    connect(EXITPushButton, SIGNAL(clicked()), this, SLOT(EXITevent()));

    lcdNumber = new QLCDNumber(this);
    lcdNumber->setGeometry(700, 5, 300, 30);
    /* 设置显示的位数 8 位 */
    lcdNumber->setDigitCount(19);
    /* 设置样式 */
    lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    /* 设置 lcd 显示为当前系统时间 */
    QDateTime time = QDateTime::currentDateTime();

    /* 设置显示的样式 */
    lcdNumber->display(time.toString("yyyy-MM-dd hh:mm:ss"));

    timer = new QTimer(this);
    /* 设置定时器 1000 毫秒发送一个 timeout()信号 */
    timer->start(1000);

    /* 信号槽连接 */
    connect(timer, SIGNAL(timeout()), this,SLOT(timerTimeOut()));
}



void Showdata::MQTTevent()
{
    //qDebug()<<"MQTT";
    this->close();
    Mqtt_client* mqttclient = new Mqtt_client;
    mqttclient->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    mqttclient->show();
}

void Showdata::MYSQLevent()
{
    //qDebug()<<"MYSQL";
    this->close();
    MySql_client* mysqlclient = new MySql_client;
    mysqlclient->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    mysqlclient->show();
}

void Showdata::WIFIevent()
{
    this->close();
    Wifi_client* wifi_client = new Wifi_client;
    wifi_client->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    wifi_client->show();
}

void Showdata::EXITevent()
{
    //qDebug()<<"EXIT";
    this->close();
    Login *login = new Login;
    login->setAttribute(Qt::WA_DeleteOnClose);          // 确保新窗口关闭时会被删除
    login->show();
}

void Showdata::timerTimeOut()
{
    /* 当定时器计时 1000 毫秒后，刷新 lcd 显示当前系统时间 */
    QDateTime time = QDateTime::currentDateTime();;
    /* 设置显示的样式 */
    lcdNumber->display(time.toString("yyyy-MM-dd hh:mm:ss"));
}
