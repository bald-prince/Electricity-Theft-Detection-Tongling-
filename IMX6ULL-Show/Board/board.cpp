#include "board.h"

Board::Board(QWidget *parent) : QMainWindow(parent)
{
    widget = new QWidget(this);
    //居中
    this->setCentralWidget(widget);
    //多页面小部件
    tabWidget = new QTabWidget();
    tabWidget->setStyleSheet("QTabBar::tab { min-width: 120px; min-height: 40px; }");

    //水平布局实例化
    hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(0);
    hBoxLayout->setSpacing(0);

    QList <QString> strTabList;
    strTabList << "LED_BEEP" << "AP3216C" << "ICM20608";

    for(int i = 0;i < 3;i++)
    {
        label[i] = new QLabel();
        tabWidget->addTab(label[i],strTabList[i]);
    }

    /*led和beep控制添加到第一个界面*/
    //led相关按键
    system("echo none > /sys/class/leds/sys-led/trigger");
    firstpage = tabWidget->widget(0);
    btnLed = new QPushButton(tr("LED开关"),firstpage);
    btnLed->setMinimumSize(200,50);
    btnLed->setGeometry(200,250,btnLed->width(),btnLed->height());
    btnLed->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");
    ledFile.setFileName("/sys/devices/platform/leds/leds/sys-led/brightness");
    getLedState();
    //beep相关按键
    btnBeep = new QPushButton(tr("BEEP开关"),firstpage);
    btnBeep->setMinimumSize(200,50);
    btnBeep->setGeometry(500,250,btnBeep->width(),btnBeep->height());
    btnBeep->setStyleSheet("background-color: green;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");
    beepFile.setFileName("/sys/devices/platform/leds/leds/beep/brightness");
    getBeepState();

    hBoxLayout->addWidget(tabWidget);
    widget->setLayout(hBoxLayout);
    /****************************************/

    /*****AP3216C控制位于第二个界面*****/
    ap3216cTimer = new QTimer(this);
    secondpage = tabWidget->widget(1);
    ap3216c = new Ap3216c(secondpage);
    connect(ap3216c, SIGNAL(ap3216cDataChanged()),this, SLOT(getAp3216cData()));
    //按键实例化及位置、大小设置
    btnAp3216cClear = new QPushButton(tr("清除数据"),secondpage);
    btnAp3216cStart = new QPushButton(tr("开始检测"),secondpage);
    btnAp3216cStop = new QPushButton(tr("停止检测"),secondpage);

    btnAp3216cClear->setGeometry(70,20,80,50);
    btnAp3216cStart->setGeometry(180,20,80,50);
    btnAp3216cStop->setGeometry(300,20,80,50);

    //数据显示框实例化及大小、位置设置
    alsData = new QTextEdit(secondpage);
    alsData->setAlignment(Qt::AlignCenter);
    alsData->setGeometry(470,10,80,40);

    psData = new QTextEdit(secondpage);
    psData->setAlignment(Qt::AlignCenter);
    psData->setGeometry(590,10,80,40);

    irData = new QTextEdit(secondpage);
    irData->setAlignment(Qt::AlignCenter);
    irData->setGeometry(710,10,80,40);

    //标签框实例化及大小、位置设置
    alsLabel = new QLabel(tr("光强度"),secondpage);
    alsLabel->setAlignment(Qt::AlignCenter);
    alsLabel->setGeometry(470,45,80,40);

    psLabel = new QLabel(tr("接近距离"),secondpage);
    psLabel->setAlignment(Qt::AlignCenter);
    psLabel->setGeometry(590,45,80,40);

    irLabel = new QLabel(tr("红外强度"),secondpage);
    irLabel->setAlignment(Qt::AlignCenter);
    irLabel->setGeometry(710,45,80,40);

    chart_init();
    //ap3216cTimer->start(500);
    /****************************************/

    //Icm20608检测位于第三个界面
    icm20608Timer = new QTimer();
    thirdpage = tabWidget->widget(2);
    icm20608 = new Icm20608(thirdpage);
    gxEdit = new QTextEdit(thirdpage);
    gxEdit->setGeometry(42,20,80,40);
    gxEdit->setAlignment(Qt::AlignCenter);

    gyEdit = new QTextEdit(thirdpage);
    gyEdit->setGeometry(162,20,80,40);
    gyEdit->setAlignment(Qt::AlignCenter);

    gzEdit = new QTextEdit(thirdpage);
    gzEdit->setGeometry(282,20,80,40);
    gzEdit->setAlignment(Qt::AlignCenter);

    axEdit = new QTextEdit(thirdpage);
    axEdit->setGeometry(402,20,80,40);
    axEdit->setAlignment(Qt::AlignCenter);

    ayEdit = new QTextEdit(thirdpage);
    ayEdit->setGeometry(522,20,80,40);
    ayEdit->setAlignment(Qt::AlignCenter);

    azEdit = new QTextEdit(thirdpage);
    azEdit->setGeometry(642,20,80,40);
    azEdit->setAlignment(Qt::AlignCenter);

    tempEdit = new QTextEdit(thirdpage);
    tempEdit->setGeometry(762,20,80,40);
    tempEdit->setAlignment(Qt::AlignCenter);

    gxLabel = new QLabel(tr("陀螺仪X轴"),thirdpage);
    gxLabel->setGeometry(42,50,80,40);
    gxLabel->setAlignment(Qt::AlignCenter);

    gyLabel = new QLabel(tr("陀螺仪Y轴"),thirdpage);
    gyLabel->setGeometry(162,50,80,40);
    gyLabel->setAlignment(Qt::AlignCenter);

    gzLabel = new QLabel(tr("陀螺仪Z轴"),thirdpage);
    gzLabel->setGeometry(282,50,80,40);
    gzLabel->setAlignment(Qt::AlignCenter);

    axLabel = new QLabel(tr("加速度X轴"),thirdpage);
    axLabel->setGeometry(402,50,80,40);
    axLabel->setAlignment(Qt::AlignCenter);

    ayLabel = new QLabel(tr("加速度Y轴"),thirdpage);
    ayLabel->setGeometry(522,50,80,40);
    ayLabel->setAlignment(Qt::AlignCenter);

    azLabel = new QLabel(tr("加速度Z轴"),thirdpage);
    azLabel->setGeometry(642,50,80,40);
    azLabel->setAlignment(Qt::AlignCenter);

    tempLabel = new QLabel(tr("温度数据"),thirdpage);
    tempLabel->setGeometry(762,50,80,40);
    tempLabel->setAlignment(Qt::AlignCenter);

    btnIcm20608Clear = new QPushButton(tr("清除数据"),thirdpage);
    btnIcm20608Clear->setGeometry(130,100,80,40);

    btnIcm20608Start = new QPushButton(tr("开始检测"),thirdpage);
    btnIcm20608Start->setGeometry(402,100,80,40);

    btnIcm20608Stop = new QPushButton(tr("停止检测"),thirdpage);
    btnIcm20608Stop->setGeometry(674,100,80,40);

    QList<QColor> colors;
    colors << "#47A4E9" << "#00B17D" << "#D64D54" << "#DEAF39" << "#A279C5" << "#009679" << "#8B658B";

    QWidget *icmwidget0 = new QWidget(thirdpage);
    icmwidget0->setGeometry(50,170,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(105,320,80,40);
    labValue[0]->setText("陀螺仪X轴");
    gaugeMini[0] = new GaugeMini();
    gaugeMini[0]->setPercentColor(colors.at(0));
    QVBoxLayout *layout0 = new QVBoxLayout(thirdpage);
    layout0->addWidget(gaugeMini[0]);
    icmwidget0->setLayout(layout0);


    QWidget *icmwidget1 = new QWidget(thirdpage);
    icmwidget1->setGeometry(250,170,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(305,320,80,40);
    labValue[0]->setText("陀螺仪Y轴");
    gaugeMini[1] = new GaugeMini();
    gaugeMini[1]->setPercentColor(colors.at(1));
    QVBoxLayout *layout1 = new QVBoxLayout(thirdpage);
    layout1->addWidget(gaugeMini[1]);
    icmwidget1->setLayout(layout1);

    QWidget *icmwidget2 = new QWidget(thirdpage);
    icmwidget2->setGeometry(450,170,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(505,320,80,40);
    labValue[0]->setText("陀螺仪Z轴");
    gaugeMini[2] = new GaugeMini();
    gaugeMini[2]->setPercentColor(colors.at(2));
    QVBoxLayout *layout2 = new QVBoxLayout(thirdpage);
    layout2->addWidget(gaugeMini[2]);
    icmwidget2->setLayout(layout2);

    QWidget *icmwidget3 = new QWidget(thirdpage);
    icmwidget3->setGeometry(650,170,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(705,320,80,40);
    labValue[0]->setText("加速度X轴");
    gaugeMini[3] = new GaugeMini();
    gaugeMini[3]->setPercentColor(colors.at(3));
    QVBoxLayout *layout3 = new QVBoxLayout(thirdpage);
    layout3->addWidget(gaugeMini[3]);
    icmwidget3->setLayout(layout3);

    QWidget *icmwidget4 = new QWidget(thirdpage);
    icmwidget4->setGeometry(130,350,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(185,500,80,40);
    labValue[0]->setText("加速度Y轴");
    gaugeMini[4] = new GaugeMini();
    gaugeMini[4]->setPercentColor(colors.at(4));
    QVBoxLayout *layout4 = new QVBoxLayout(thirdpage);
    layout4->addWidget(gaugeMini[4]);
    icmwidget4->setLayout(layout4);

    QWidget *icmwidget5 = new QWidget(thirdpage);
    icmwidget5->setGeometry(350,350,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(400,500,80,40);
    labValue[0]->setText("加速度Z轴");
    gaugeMini[5] = new GaugeMini();
    gaugeMini[5]->setPercentColor(colors.at(5));
    QVBoxLayout *layout5 = new QVBoxLayout(thirdpage);
    layout5->addWidget(gaugeMini[5]);
    icmwidget5->setLayout(layout5);

    QWidget *icmwidget6 = new QWidget(thirdpage);
    icmwidget6->setGeometry(570,350,180,180);
    labValue[0] = new QLabel(thirdpage);
    labValue[0]->setGeometry(630,500,80,40);
    labValue[0]->setText("温度数据");
    gaugeMini[6] = new GaugeMini();
    gaugeMini[6]->setPercentColor(colors.at(6));
    QVBoxLayout *layout6 = new QVBoxLayout(thirdpage);
    layout6->addWidget(gaugeMini[6]);
    icmwidget6->setLayout(layout6);


    icm20608Timer->start(200);
    /****************************************/

    //信号与槽连接
    connect(btnLed,SIGNAL(clicked()),this,SLOT(btnLedClicked()));
    connect(btnBeep,SIGNAL(clicked()),this,SLOT(btnBeepClicked()));

    connect(btnAp3216cStart,SIGNAL(clicked()),this,SLOT(btnAp3216cStartClicked()));
    connect(btnAp3216cStop,SIGNAL(clicked()),this,SLOT(btnAp3216cStopClicked()));
    connect(btnAp3216cClear,SIGNAL(clicked()),this,SLOT(btnAp3216cClearClicked()));
    //connect(ap3216cTimer,SIGNAL(timeout()),this,SLOT(ap3216cTimerTimeOut()));

    connect(btnIcm20608Clear,SIGNAL(clicked()),this,SLOT(btnIcm20608ClearClicked()));
    connect(btnIcm20608Start,SIGNAL(clicked()),this,SLOT(btnIcm20608StartClicked()));
    connect(btnIcm20608Stop,SIGNAL(clicked()),this,SLOT(btnIcm20608StopClicked()));
    connect(icm20608Timer,SIGNAL(timeout()),this,SLOT(icm20608TimerTimeout()));
    connect(icm20608, SIGNAL(icm20608DataChanged()),this, SLOT(getIcm20608Data()));

    connect(gxEdit,SIGNAL(textChanged()),this,SLOT(gxEditSetvalue()));
    connect(gyEdit,SIGNAL(textChanged()),this,SLOT(gyEditSetvalue()));
    connect(gzEdit,SIGNAL(textChanged()),this,SLOT(gzEditSetvalue()));
    connect(axEdit,SIGNAL(textChanged()),this,SLOT(axEditSetvalue()));
    connect(ayEdit,SIGNAL(textChanged()),this,SLOT(ayEditSetvalue()));
    connect(azEdit,SIGNAL(textChanged()),this,SLOT(azEditSetvalue()));
    connect(tempEdit,SIGNAL(textChanged()),this,SLOT(tempEditSetvalue()));
    /****************************************/

}

Board::~Board()
{

}
//图表初始化
void Board::chart_init()
{
    //图表显示
    //1.设置坐标轴
    axisX = new QValueAxis();
    axisY = new QValueAxis();
    axisX->setLabelsColor(Qt::white);
    axisY->setLabelsColor(Qt::white);
    axisX->setLinePenColor(Qt::white);
    axisY->setLinePenColor(Qt::white);
    axisX->setTitleBrush(QBrush(Qt::white));
    axisY->setTitleBrush(QBrush(Qt::white));

    axisX->setTitleText("X-label");
    axisY->setTitleText("Y-label");
    axisX->setMin(0);
    axisY->setMax(-1);
    axisX->setMax(AXIS_MAX_X);
    axisY->setMax(AXIS_MAX_Y);

    //创建als曲线对象
    alsSplineSeries = new QSplineSeries();
    alsSplineSeries->setPointsVisible(true);                     // 设置数据点可见
    alsSplineSeries->setName("光强度");                         // 图例名称

    //创建ir曲线对象
    irSplineSeries = new QSplineSeries();
    irSplineSeries->setPointsVisible(true);                    // 设置数据点可见
    irSplineSeries->setName("红外强度");                         // 图例名称

    //创建ps曲线对象
    psSplineSeries = new QSplineSeries();
    psSplineSeries->setPointsVisible(true);                   // 设置数据点可见
    psSplineSeries->setName("接近距离");                         // 图例名称

    chart = new QChart();                                        // 创建图表对象
    chartView = new QChartView(secondpage);
    chartView->setGeometry(0,80,874,480);
    chart->setBackgroundBrush(QBrush(Qt::black));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setBrush(QBrush(Qt::white));
    chart->legend()->setLabelColor(Qt::white);

    chart->addAxis(axisY, Qt::AlignLeft);                      // 将X轴添加到图表上
    chart->addAxis(axisX, Qt::AlignBottom);                    // 将Y轴添加到图表上
    chart->addSeries(alsSplineSeries);                              // 将als曲线对象添加到图表上
    chart->addSeries(irSplineSeries);                              // 将ir曲线对象添加到图表上
    chart->addSeries(psSplineSeries);                              // 将ps曲线对象添加到图表上

    chart->setAnimationOptions(QChart::SeriesAnimations);        // 动画：能使曲线绘制显示的更平滑，过渡效果更好看

    alsSplineSeries->attachAxis(axisX);                             // als曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    alsSplineSeries->attachAxis(axisY);                             // als曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后
    //ir曲线关联到X，Y轴
    irSplineSeries->attachAxis(axisX);
    irSplineSeries->attachAxis(axisY);

    //ps曲线关联到X，Y轴
    psSplineSeries->attachAxis(axisX);
    psSplineSeries->attachAxis(axisY);

    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

//led相关函数
bool Board::getLedState()
{
    //如果文件不存在
    if(!ledFile.exists())
        return false;
    if(!ledFile.open(QIODevice::ReadWrite))
        qDebug() << ledFile.errorString();

    QTextStream in(&ledFile);
    QString buf = in.readLine();

    //qDebug() << "ledbuf:" << buf;
    ledFile.close();

    if(buf == "1")
    {
        btnLed->setText("LED开");
        return true;
    }
    else
    {
        btnLed->setText("LED关");
        return false;
    }

}
void Board::setLedState()
{
    ledstate = getLedState();

    //如果文件不存在
    if(!ledFile.exists())
        return;

    if(!ledFile.open(QIODevice::ReadWrite))
        qDebug() << ledFile.errorString();

    QByteArray ledbuf[2] = {"0","1"};

    if(ledstate)
        ledFile.write(ledbuf[0]);
    else
        ledFile.write(ledbuf[1]);

    ledFile.close();

    getLedState();

}
//beep相关函数
bool Board::getBeepState()
{
    //如果文件不存在
    if(!beepFile.exists())
        return false;

    if(!beepFile.open(QIODevice::ReadWrite))
        qDebug() << beepFile.errorString();

    QTextStream in(&beepFile);

    QString buf = in.readLine();

    //qDebug() << "beepbuf:" << buf;
    beepFile.close();

    if(buf == "1")
    {
        btnBeep->setText("BEEP开");
        return true;
    }
    else
    {
        btnBeep->setText("BEEP关");
        return false;
    }

}
void Board::setBeepState()
{
    beepstate = getBeepState();

    //如果文件不存在
    if(!beepFile.exists())
        return;

    if(!beepFile.open(QIODevice::ReadWrite))
        qDebug() << beepFile.errorString();

    QByteArray beepbuf[2] = {"0","1"};

    if(beepstate)
        beepFile.write(beepbuf[0]);
    else
        beepFile.write(beepbuf[1]);

    beepFile.close();

    getBeepState();
}
//获取ap3216c数据
void Board::getAp3216cData()
{
    static QString als = ap3216c->alsData();
    if (als != ap3216c->alsData())
        als = ap3216c->alsData();

    static QString ps = ap3216c->psData();
    if (ps != ap3216c->psData())
        ps = ap3216c->psData();

    static QString ir = ap3216c->irData();
    if (ir != ap3216c->irData())
        ir = ap3216c->irData();
    //更新数据到文本显示框
    alsData->setText(als);
    irData->setText(ir);
    psData->setText(ps);

    //画曲线代码
    if(pointCount > AXIS_MAX_X)
    {
        alsSplineSeries->remove(0);
        axisX->setMin(pointCount - AXIS_MAX_X);
        axisX->setMax(pointCount);                    // 更新X轴范围
    }
    alsSplineSeries->append(QPointF(pointCount, als.toInt()));  // 更新显示
    irSplineSeries->append(QPointF(pointCount, ir.toInt()));  // 更新显示
    psSplineSeries->append(QPointF(pointCount, ps.toInt()));  // 更新显示

    pointCount++;
}

//void Board::ap3216cTimerTimeOut()
//{
//    getAp3216cData();
//}

void Board::getIcm20608Data()
{
    static QString gx = icm20608->gxData();
    if (gx != icm20608->gxData())
        gx = icm20608->gxData();

    static QString gy = icm20608->gyData();
    if (gy != icm20608->gyData())
        gy = icm20608->gyData();

    static QString gz = icm20608->gzData();
    if (gz != icm20608->gzData())
        gz = icm20608->gzData();

    static QString ax = icm20608->axData();
    if (ax != icm20608->axData())
        ax = icm20608->axData();

    static QString ay = icm20608->ayData();
    if (ay != icm20608->ayData())
        ay = icm20608->ayData();

    static QString az = icm20608->azData();
    if (az != icm20608->azData())
        az = icm20608->azData();

    static QString temp_data = icm20608->tempData();
    if (temp_data != icm20608->tempData())
        temp_data = icm20608->tempData();

    gxEdit->setText(gx);
    gyEdit->setText(gy);
    gzEdit->setText(gz);
    axEdit->setText(ax);
    ayEdit->setText(ay);
    azEdit->setText(az);
    tempEdit->setText(temp_data);

}

void Board::btnIcm20608ClearClicked()
{
    gxEdit->clear();
    gyEdit->clear();
    gzEdit->clear();
    axEdit->clear();
    ayEdit->clear();
    azEdit->clear();
    tempEdit->clear();
    icm20608Timer->stop();
}
//Icm20608按键相关槽函数
void Board::btnIcm20608StartClicked()
{
    icm20608Timer->start(200);
    icm20608->setCapture(true);
}
void Board::btnIcm20608StopClicked()
{
    icm20608->setCapture(false);
}

void Board::icm20608TimerTimeout()
{
    getIcm20608Data();
}

void Board::gxEditSetvalue()
{
    double value = gxEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[0]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[0]->minValue)
        value = gaugeMini[0]->minValue;

    else if (value > gaugeMini[0]->maxValue)
        value = gaugeMini[0]->maxValue;

    gaugeMini[0]->value = value;
    emit gaugeMini[0]->valueChanged(value);
    gaugeMini[0]->update();
}

void Board::gyEditSetvalue()
{
    double value = gyEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[1]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[1]->minValue)

        value = gaugeMini[1]->minValue;
     else if (value > gaugeMini[1]->maxValue)
        value = gaugeMini[1]->maxValue;

    gaugeMini[1]->value = value;
    emit gaugeMini[1]->valueChanged(value);
    gaugeMini[1]->update();
}

void Board::gzEditSetvalue()
{
    double value = gzEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[2]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[2]->minValue)
        value = gaugeMini[2]->minValue;
    else if (value > gaugeMini[2]->maxValue)
        value = gaugeMini[2]->maxValue;

    gaugeMini[2]->value = value;
    emit gaugeMini[2]->valueChanged(value);
    gaugeMini[2]->update();
}

void Board::axEditSetvalue()
{
    double value = axEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[3]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[3]->minValue)
        value = gaugeMini[3]->minValue;
    else if (value > gaugeMini[3]->maxValue)
        value = gaugeMini[3]->maxValue;

    gaugeMini[3]->value = value;
    emit gaugeMini[3]->valueChanged(value);
    gaugeMini[3]->update();
}

void Board::ayEditSetvalue()
{
    double value = ayEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[4]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[4]->minValue)
        value = gaugeMini[4]->minValue;
    else if (value > gaugeMini[4]->maxValue)
        value = gaugeMini[4]->maxValue;

    gaugeMini[4]->value = value;
    emit gaugeMini[4]->valueChanged(value);
    gaugeMini[4]->update();
}

void Board::azEditSetvalue()
{
    double value = azEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[5]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[5]->minValue)
        value = gaugeMini[5]->minValue;
    else if (value > gaugeMini[5]->maxValue)
        value = gaugeMini[5]->maxValue;

    gaugeMini[5]->value = value;
    emit gaugeMini[5]->valueChanged(value);
    gaugeMini[5]->update();
}

void Board::tempEditSetvalue()
{
    double value = tempEdit->toPlainText().toDouble();
    //值和当前值一致则处理
    if (value == gaugeMini[6]->value)
        return;

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < gaugeMini[6]->minValue)
        value = gaugeMini[6]->minValue;
    else if (value > gaugeMini[6]->maxValue)
        value = gaugeMini[6]->maxValue;

    gaugeMini[6]->value = value;
    emit gaugeMini[6]->valueChanged(value);
    gaugeMini[6]->update();
}
//led槽函数
void Board::btnLedClicked()
{
    setLedState();
}
//beep槽函数
void Board::btnBeepClicked()
{
    setBeepState();
}
//ap3216c相关槽函数
void Board::btnAp3216cClearClicked()
{
    psData->clear();
    irData->clear();
    alsData->clear();
    alsSplineSeries->clear();
    irSplineSeries->clear();
    psSplineSeries->clear();
    axisX->setMin(0);
    axisX->setMax(AXIS_MAX_X);
    pointCount = 0;
}
void Board::btnAp3216cStartClicked()
{
    ap3216cTimer->start(200);
    ap3216c->setCapture(true);
}
void Board::btnAp3216cStopClicked()
{
    ap3216c->setCapture(false);
    ap3216cTimer->stop();
}
