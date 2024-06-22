#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QPlainTextEdit>
#include <qmqttclient.h>
#include <QtCore/QDateTime>
#include <QtWidgets/QMessageBox>
#include <QString>
#include <QList>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QChartView>
#include <QLineSeries>
#include <QTimer>
#include <QChart>
#include "json.h"
#include <QDebug>
#include "cJSON.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTouchEvent>
#include <QCheckBox>
#include <QDebug>
#include <QTableWidget>
#include <QPalette>
#include <QPixmap>

//光添加头文件QChartView还不够，还需要引入QChart的命名空间
QT_CHARTS_USE_NAMESPACE

class Mqtt_client : public QMainWindow
{
    Q_OBJECT
public:
    explicit Mqtt_client(QWidget *parent = nullptr);
    //void mqtt_chart_init();
    void mqtt_chart_init_select();      //图标显示初始化
private:
    //页面相关
    QWidget *widget;            //多窗口
    QTabWidget *tabWidget;      //标题
    QHBoxLayout *hBoxLayout;    //布局
    QLabel *label[3];           //文字标签
    QWidget *firstpage;         //第一个页面：mqtt连接页面
    QWidget *secondpage;        //第二个页面：文字显示页面
    QWidget *thirdpage;         //第三个页面：曲线显示页面
    QLabel *labValue[6];        //标签
    QList<QLabel *> labs;

    //按键
    QPushButton *btnConnect;        //连接按钮
    QPushButton *btnSubscribe;      //订阅话题按钮
    QPushButton *btnPublish;        //发布话题按钮
    QPushButton *btnclear;          //清除按钮
    QPushButton *btnTextClear;      //文本显示界面清除按钮

    QPushButton *btnFirstExit;      //第一个界面退出按钮
    QPushButton *btnSecondExit;     //第二个界面退出按钮
    QPushButton *btnThridExit;      //第二个界面退出按钮

    QPushButton *selectbtnclear;            //曲线显示界面清除按钮
    QPushButton *selectbtnStopMqtt;         //曲线显示界面暂停按钮
    QPushButton *selectbtnStartMqtt;        //曲线显示界面开始按钮

    QLineEdit *lineEditHost;                //IP输入框
    QLineEdit *lineEditPort;                //端口号输入框
    QLineEdit *lineEditTopicSub;            //订阅话题输入框
    QLineEdit *lineEditTopicPub;            //发布话题输入框
    QLineEdit *lineEditMsg;                 //发布消息输入框
    QLineEdit *lineEditQos;                 //发布消息qos输入框
    QPlainTextEdit *plainTextEditLog;       //文本显示框
    QLabel *labelHost;                      //host标签
    QLabel *labelPort;                      //端口号标签
    QLabel *labelTopicSub;                  //定义话题标签
    QLabel *labelTopicPub;                  //发布话题标签
    QLabel *labelMsg;                       //发布消息标签
    QLabel *labelQos;                       //qos标签
    bool selectappendState;                 //曲线显示添加标志位

    QCheckBox *checkbox[9];                 //通道选择


    QChart *selectmqtt_chart;        //图表对象

    QValueAxis *mqtt_axisX, *mqtt_axisY;//横纵坐标轴对象

    QSplineSeries *selectchSplineSeries[9];     //曲线
    QChartView *selectmqtt_chartView;

    const int MQTT_AXIS_MAX_X = 50, MQTT_AXIS_MAX_Y = 100;//横纵坐标最大显示范围

    int selectmqtt_pointCount = 0; //用来记录数据点数

    QVector<double> currentValues;      //订阅的数据
    double longitude;                   //经度
    double latitude;                    //纬度


public slots:
    void setClientPort(int port);       //端口号选择

private slots:
    void updateLogStateChange();
    void connect_client();      //连接mqtt客户端槽函数
    void Subscribe_topic();     //订阅话题槽函数
    void publish_msg();         //发布消息槽函数
    void btnTextClearClicked(); //文本显示清除槽函数

    void btnFirstExitEvent();       //第一个页面退出槽函数
    void btnSecondExitEvent();      //第二个页面退出槽函数
    void btnThridExitEvent();       //第三个页面退出槽函数

    void selectbtnStopMqttClicked();    //图表显示停止槽函数
    void selectbtnStartMqttClicked();   //图表开始停止槽函数
    void selectbtnclearClicked();       //图表清除停止槽函数

    //九个通道选择槽函数
    void checkboxStateChanged(int);
    void checkbox1StateChanged(int);
    void checkbox2StateChanged(int);
    void checkbox3StateChanged(int);
    void checkbox4StateChanged(int);
    void checkbox5StateChanged(int);
    void checkbox6StateChanged(int);
    void checkbox7StateChanged(int);
    void checkbox8StateChanged(int);

private:
    QMqttClient *m_client;      //mqtt客户端


};

#endif // MQTT_CLIENT_H
