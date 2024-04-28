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

//光添加头文件QChartView还不够，还需要引入QChart的命名空间
QT_CHARTS_USE_NAMESPACE

class Mqtt_client : public QMainWindow
{
    Q_OBJECT
public:
    explicit Mqtt_client(QWidget *parent = nullptr);
    void mqtt_chart_init();
    void mqtt_chart_init_select();
private:
    //页面相关
    QWidget *widget;
    QTabWidget *tabWidget;
    QHBoxLayout *hBoxLayout;
    QLabel *label[3];
    QWidget *firstpage;
    QWidget *secondpage;
    QWidget *thirdpage;
    QWidget *fourthpage;
    QLabel *labValue[6];
    QList<QLabel *> labs;

    //按键
    QPushButton *btnConnect;
    QPushButton *btnSubscribe;
    QPushButton *btnPublish;
    QPushButton *btnclear;
    QPushButton *btnTextClear;
    QPushButton *btnStopMqtt;
    QPushButton *btnStartMqtt;

    QPushButton *selectbtnclear;
    QPushButton *selectbtnStopMqtt;
    QPushButton *selectbtnStartMqtt;

    QLineEdit *lineEditHost;
    QLineEdit *lineEditPort;
    QLineEdit *lineEditTopicSub;
    QLineEdit *lineEditTopicPub;
    QLineEdit *lineEditMsg;
    QLineEdit *lineEditQos;
    QPlainTextEdit *plainTextEditLog;
    QLabel *labelHost;
    QLabel *labelPort;
    QLabel *labelTopicSub;
    QLabel *labelTopicPub;
    QLabel *labelMsg;
    QLabel *labelQos;
    bool appendState;
    bool selectappendState;

    QCheckBox *checkbox[8];

    //图表对象
    QChart *mqtt_chart;
    QChart *selectmqtt_chart;
    //横纵坐标轴对象
    QValueAxis *mqtt_axisX, *mqtt_axisY;
    QSplineSeries *chSplineSeries[8];
    QChartView *mqtt_chartView;

    QSplineSeries *selectchSplineSeries[8];
    QChartView *selectmqtt_chartView;
    //横纵坐标最大显示范围
    const int MQTT_AXIS_MAX_X = 50, MQTT_AXIS_MAX_Y = 100;
    //用来记录数据点数
    int mqtt_pointCount = 0;
    int selectmqtt_pointCount = 0;

    QVector<double> currentValues;


public slots:
    void setClientPort(int port);

private slots:
    void updateLogStateChange();
    void connect_client();
    void Subscribe_topic();
    void publish_msg();
    void btnclearClicked();
    void btnTextClearClicked();
    void btnStopMqttClicked();
    void btnStartMqttClicked();

    void selectbtnStopMqttClicked();
    void selectbtnStartMqttClicked();
    void selectbtnclearClicked();

    void checkboxStateChanged(int);
    void checkbox1StateChanged(int);
    void checkbox2StateChanged(int);
    void checkbox3StateChanged(int);
    void checkbox4StateChanged(int);
    void checkbox5StateChanged(int);
    void checkbox6StateChanged(int);
    void checkbox7StateChanged(int);

private:
    QMqttClient *m_client;

};

#endif // MQTT_CLIENT_H
