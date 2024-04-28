#ifndef BOARD_H
#define BOARD_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include <QDebug>
#include <QWidget>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QString>
#include <QList>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QChartView>
#include <QLineSeries>
#include <QTimer>
#include "ap3216c.h"
#include "icm20608.h"
#include "gaugemini.h"

//光添加头文件QChartView还不够，还需要引入QChart的命名空间
QT_CHARTS_USE_NAMESPACE

class Board : public QMainWindow
{
    Q_OBJECT
public:
    explicit Board(QWidget *parent = nullptr);
    ~Board();
    void chart_init();

private:
    //页面相关
    QWidget *widget;
    QTabWidget *tabWidget;
    QHBoxLayout *hBoxLayout;
    QLabel *label[3];
    QWidget *firstpage;
    QWidget *secondpage;
    QWidget *thirdpage;
    GaugeMini *gaugeMini[6];
    QLabel *labValue[6];

    QList<GaugeMini *> gauges;
    QList<QLabel *> labs;

    //led相关
    QFile ledFile;
    QPushButton *btnLed;
    bool ledstate;
    bool getLedState();
    void setLedState();

    //beep相关
    QFile beepFile;
    QPushButton *btnBeep;
    bool beepstate;
    bool getBeepState();
    void setBeepState();

    //AP3216C相关
    QPushButton *btnAp3216cClear;
    QPushButton *btnAp3216cStart;
    QPushButton *btnAp3216cStop;
    QTextEdit *alsData;
    QTextEdit *psData;
    QTextEdit *irData;
    QLabel *alsLabel;
    QLabel *psLabel;
    QLabel *irLabel;
    //AP3216c对象
    Ap3216c *ap3216c;
    //图表对象
    QChart *chart;
    QTimer *ap3216cTimer;
    //横纵坐标轴对象
    QValueAxis *axisX, *axisY;
    //曲线图对象
    QSplineSeries *alsSplineSeries;        //als曲线
    QSplineSeries *irSplineSeries;       //ir曲线
    QSplineSeries *psSplineSeries;       //ps曲线
    QChartView *chartView;
    //横纵坐标最大显示范围
    const int AXIS_MAX_X = 100, AXIS_MAX_Y = 1200;
    //用来记录数据点数
    int pointCount = 0;
    //Icm20608相关
    Icm20608 *icm20608;
    QTimer *icm20608Timer;
    QTextEdit *gxEdit;
    QTextEdit *gyEdit;
    QTextEdit *gzEdit;
    QTextEdit *axEdit;
    QTextEdit *ayEdit;
    QTextEdit *azEdit;
    QTextEdit *tempEdit;
    QLabel *gxLabel;
    QLabel *gyLabel;
    QLabel *gzLabel;
    QLabel *axLabel;
    QLabel *ayLabel;
    QLabel *azLabel;
    QLabel *tempLabel;
    QPushButton *btnIcm20608Start;
    QPushButton *btnIcm20608Stop;
    QPushButton *btnIcm20608Clear;

private slots:
    //led相关槽函数
    void btnLedClicked();
    //beep相关槽函数
    void btnBeepClicked();
    //AP3216C相关槽函数
    void btnAp3216cClearClicked();
    void btnAp3216cStartClicked();
    void btnAp3216cStopClicked();
    void getAp3216cData();
    //void ap3216cTimerTimeOut();
    //获取icm20608传感器数据
    void getIcm20608Data();
    //Icm20608相关槽函数
    void btnIcm20608ClearClicked();
    void btnIcm20608StartClicked();
    void btnIcm20608StopClicked();
    void icm20608TimerTimeout();
    void gxEditSetvalue();
    void gyEditSetvalue();
    void gzEditSetvalue();
    void axEditSetvalue();
    void ayEditSetvalue();
    void azEditSetvalue();
    void tempEditSetvalue();

};

#endif // BOARD_H
