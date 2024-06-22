#include "mqtt_client.h"
#include "showdata.h"

Mqtt_client::Mqtt_client(QWidget *parent) : QMainWindow(parent)
{
    //背景显示
    this->setGeometry(0, 0, 1024, 600);
    QPixmap backgroundPixmap(":/icons/desk.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, backgroundPixmap);
    this->setPalette(palette);
    this->setAutoFillBackground(true); // 确保窗口自动填充背景
    //页面初始化
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

    //多页面标题
    QList <QString> strTabList;
    strTabList << "连接信息" << "文本显示" << "图表显示";

    for(int i = 0; i < 3; i++)
    {
        label[i] = new QLabel();
        tabWidget->addTab(label[i],strTabList[i]);
    }

    //第一个页面
    //实例化
    firstpage = tabWidget->widget(0);
    btnConnect = new QPushButton(tr("连接"),firstpage);
    btnSubscribe = new QPushButton(tr("订阅"),firstpage);
    btnPublish = new QPushButton(tr("发布"),firstpage);
    btnFirstExit = new QPushButton(tr("退出"),firstpage);

    labelHost = new QLabel(tr("Host"),firstpage);
    labelPort = new QLabel(tr("Port"),firstpage);
    labelTopicSub = new QLabel(tr("SubTopic"),firstpage);
    labelTopicPub = new QLabel(tr("PubTopic"),firstpage);
    labelMsg = new QLabel(tr("Msg"),firstpage);
    labelQos = new QLabel(tr("Qos"),firstpage);

    lineEditHost = new QLineEdit(firstpage);
    lineEditPort = new QLineEdit(firstpage);
    lineEditTopicSub = new QLineEdit(firstpage);
    lineEditTopicPub = new QLineEdit(firstpage);
    lineEditMsg = new QLineEdit(firstpage);
    lineEditQos = new QLineEdit(firstpage);

    lineEditHost->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditPort->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditTopicSub->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditTopicPub->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditMsg->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditQos->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );


    lineEditHost->setText("116.198.227.107");
    lineEditPort->setText("1883");
    lineEditTopicSub->setText("sys/bc20/pubtopic");

    lineEditTopicPub->setText("sys/bc20/subtopic");
    lineEditMsg->setText("hello_world");
    //设置位置及大小

    labelHost->setGeometry(100,10,140,40);
    labelPort->setGeometry(540,10,140,40);
    lineEditHost->setGeometry(100,50,370,70);
    lineEditPort->setGeometry(540,50,370,70);

    labelTopicSub->setGeometry(100,140,140,40);
    labelQos->setGeometry(540,140,140,40);
    lineEditTopicSub->setGeometry(100,180,370,70);
    lineEditQos->setGeometry(540,180,370,70);

    labelTopicPub->setGeometry(100,280,140,40);
    labelMsg->setGeometry(540,280,140,40);
    lineEditTopicPub->setGeometry(100,320,370,70);
    lineEditMsg->setGeometry(540,320,370,70);

    btnConnect->setGeometry(115,450,100,50);
    btnSubscribe->setGeometry(340,450,100,50);
    btnPublish->setGeometry(565,450,100,50);
    btnFirstExit->setGeometry(790,450,100,50);

    btnConnect->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");
    btnSubscribe->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");
    btnPublish->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");
    btnFirstExit->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    //第二个页面
    secondpage = tabWidget->widget(1);
    plainTextEditLog = new QPlainTextEdit(secondpage);
    plainTextEditLog->setGeometry(10,10,890,540);
    plainTextEditLog->setStyleSheet("QPlainTextEdit{border: 1px solid gray;border-radius: 10px;}");

    btnTextClear = new QPushButton(tr("清除"),secondpage);
    btnTextClear->setGeometry(910,200,100,50);
    btnTextClear->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    btnSecondExit = new QPushButton(tr("退出"),secondpage);
    btnSecondExit->setGeometry(910,350,100,50);
    btnSecondExit->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    m_client = new QMqttClient(this);
    m_client->setHostname(lineEditHost->text());
    m_client->setPort(lineEditPort->text().toUInt());
    m_client->setClientId("k0hztVSi32P.mqtt_test|securemode=2,signmethod=hmacsha256,timestamp=1709770574822|");
    m_client->setUsername("mqtt_test&k0hztVSi32P");
    m_client->setPassword("0d739648e91d7241ee346574768f80d2e3ea508782c9031c72d6206768ecee05");

    connect(btnTextClear,SIGNAL(clicked()),this,SLOT(btnTextClearClicked()));
    connect(m_client, &QMqttClient::stateChanged, this, &Mqtt_client::updateLogStateChange);
    connect(btnFirstExit, SIGNAL(clicked()), this, SLOT(btnFirstExitEvent()));
    connect(btnSecondExit, SIGNAL(clicked()), this, SLOT(btnSecondExitEvent()));

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String("---Received Topic:  ")
                + topic.name()
                + QLatin1String(" Message:  ")
                + message
                + QLatin1Char('\n');
        plainTextEditLog->insertPlainText(content);
    });


    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        QString stringData = QString::fromUtf8(message);
        // 将 JSON 字符串转换为 QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(stringData.toUtf8());
        // 检查 JSON 是否有效
        if (jsonDoc.isNull())
            return;
        // 获取 JSON 对象
        QJsonObject jsonObj = jsonDoc.object();
        // 遍历 JSON 对象，提取所需数据
        QJsonArray dataArray;
        for (const auto& item : jsonObj["power_data"].toArray())
        {
            if (item.isObject())
            {
                QJsonObject obj = item.toObject();
                // 提取 data 数组
                if (obj.contains("data"))
                    dataArray = obj["data"].toArray();

                // 提取 location 中的经度和纬度
                if (obj.contains("location"))
                {
                    QJsonObject location = obj["location"].toObject();
                    if (location.contains("lon"))
                        longitude = location["lon"].toDouble();
                    if (location.contains("lat"))
                        latitude = location["lat"].toDouble();
                }
            }
        }
        // 存储数据到数组中
        for (const auto& value : dataArray)
            currentValues.append(value.toDouble());

        //画曲线代码
        if(selectmqtt_pointCount > MQTT_AXIS_MAX_X)
        {
            selectchSplineSeries[0]->remove(0);
            mqtt_axisX->setMin(selectmqtt_pointCount - MQTT_AXIS_MAX_X);
            mqtt_axisX->setMax(selectmqtt_pointCount);                    // 更新X轴范围
        }

        if(selectappendState)
        {
            int temp = INT_MIN;
            for(int i = 0;i < 9; i++)
            {
                if(currentValues.at(i) > temp)
                    temp = currentValues.at(i);
                else
                    mqtt_axisY->setMax(currentValues.at(i) + 30);

                mqtt_axisY->setMax(temp + 30);
                selectchSplineSeries[i]->append(QPointF(selectmqtt_pointCount, currentValues.at(i)));  // 更新显示
            }

            selectmqtt_pointCount++;
        }
    });

    connect(lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    connect(btnConnect, SIGNAL(clicked()), this, SLOT(connect_client()));
    connect(btnSubscribe, SIGNAL(clicked()), this, SLOT(Subscribe_topic()));
    connect(btnPublish, SIGNAL(clicked()), this, SLOT(publish_msg()));

    QTextCursor cursor = plainTextEditLog->textCursor();
    cursor.movePosition(QTextCursor::End);

    // 设置新的光标并确保它是可见的
    plainTextEditLog->setTextCursor(cursor);
    plainTextEditLog->ensureCursorVisible();

    //第三个页面
    thirdpage = tabWidget->widget(2);
    mqtt_chart_init_select();


    hBoxLayout->addWidget(tabWidget);
    widget->setLayout(hBoxLayout);
}

void Mqtt_client::mqtt_chart_init_select()
{
    selectbtnStartMqtt = new QPushButton(tr("开始"),thirdpage);
    selectbtnStartMqtt->setGeometry(910,0,100,40);
    selectbtnStartMqtt->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    selectbtnStopMqtt = new QPushButton(tr("暂停"),thirdpage);
    selectbtnStopMqtt->setGeometry(910,50,100,40);
    selectbtnStopMqtt->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    selectbtnclear = new QPushButton(tr("清除"),thirdpage);
    selectbtnclear->setGeometry(910,100,100,40);
    selectbtnclear->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    btnThridExit = new QPushButton(tr("退出"),thirdpage);
    btnThridExit->setGeometry(910,150,100,40);
    btnThridExit->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    checkbox[0] = new QCheckBox(tr("CH1_V"),thirdpage);
    checkbox[1] = new QCheckBox(tr("CH2_V"),thirdpage);
    checkbox[2] = new QCheckBox(tr("CH3_V"),thirdpage);
    checkbox[3] = new QCheckBox(tr("CH1_A"),thirdpage);
    checkbox[4] = new QCheckBox(tr("CH2_A"),thirdpage);
    checkbox[5] = new QCheckBox(tr("CH3_A"),thirdpage);
    checkbox[6] = new QCheckBox(tr("CH1_P"),thirdpage);
    checkbox[7] = new QCheckBox(tr("CH2_P"),thirdpage);
    checkbox[8] = new QCheckBox(tr("CH3_P"),thirdpage);

    for(int i = 0;i < 9;i++)
    {
        checkbox[i]->setGeometry(910,200 + 40 * i,100,40);
        checkbox[i]->setCheckState(Qt::Unchecked);
    }

    mqtt_axisX = new QValueAxis();
    mqtt_axisY = new QValueAxis();
    mqtt_axisX->setLabelsColor(Qt::black);
    mqtt_axisY->setLabelsColor(Qt::black);
    mqtt_axisX->setLinePenColor(Qt::black);
    mqtt_axisY->setLinePenColor(Qt::black);
    mqtt_axisX->setTitleBrush(QBrush(Qt::black));
    mqtt_axisY->setTitleBrush(QBrush(Qt::black));

    mqtt_axisX->setTitleText("X-label");
    mqtt_axisY->setTitleText("电压 电流 功率/V A P");
    mqtt_axisX->setMin(0);
    mqtt_axisY->setMax(0);
    mqtt_axisX->setMax(MQTT_AXIS_MAX_X);
    mqtt_axisY->setMax(MQTT_AXIS_MAX_Y);

    QList<QColor> colors;
    colors << "#47A4E9" << "#00B17D" << "#D64D54" << "#DEAF39" << "#A279C5" << "#009679" << "#0B658B" << "#8B008B" << "#0B008B";

    for(int i = 0;i < 9;i++)
    {
        selectchSplineSeries[i] = new QSplineSeries();
        QPen pen(colors.at(i));
        pen.setWidth(3);
        selectchSplineSeries[i]->setPen(pen);
        selectchSplineSeries[i]->setPointsVisible(true);
        selectchSplineSeries[i]->setVisible(false);
    }

    selectchSplineSeries[0]->setName("CH1V");
    selectchSplineSeries[1]->setName("CH2V");
    selectchSplineSeries[2]->setName("CH3V");

    selectchSplineSeries[3]->setName("CH1A");
    selectchSplineSeries[4]->setName("CH2A");
    selectchSplineSeries[5]->setName("CH3A");

    selectchSplineSeries[6]->setName("CH1P");
    selectchSplineSeries[7]->setName("CH2P");
    selectchSplineSeries[8]->setName("CH3P");

    selectmqtt_chart = new QChart();                                        // 创建图表对象
    selectmqtt_chartView = new QChartView(thirdpage);
    selectmqtt_chartView->setGeometry(0,0,900,560);
    selectmqtt_chart->setBackgroundBrush(QBrush(Qt::white));
    selectmqtt_chart->setTitleBrush(QBrush(Qt::black));
    selectmqtt_chart->legend()->setBrush(QBrush(Qt::black));
    selectmqtt_chart->legend()->setLabelColor(Qt::black);

    selectmqtt_chart->addAxis(mqtt_axisY, Qt::AlignLeft);                      // 将X轴添加到图表上
    selectmqtt_chart->addAxis(mqtt_axisX, Qt::AlignBottom);                    // 将Y轴添加到图表上

    for(int i = 0;i < 9;i++)
    {
        selectmqtt_chart->addSeries(selectchSplineSeries[i]);
        selectchSplineSeries[i]->attachAxis(mqtt_axisX);  //als曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
        selectchSplineSeries[i]->attachAxis(mqtt_axisY);  // als曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后
    }

    selectmqtt_chart->setAnimationOptions(QChart::SeriesAnimations);        // 动画：能使曲线绘制显示的更平滑，过渡效果更好看

    selectmqtt_chartView->setChart(selectmqtt_chart);
    selectmqtt_chartView->setRenderHint(QPainter::Antialiasing);

    connect(selectbtnclear,SIGNAL(clicked()),this,SLOT(selectbtnclearClicked()));
    connect(selectbtnStopMqtt,SIGNAL(clicked()),this,SLOT(selectbtnStopMqttClicked()));
    connect(selectbtnStartMqtt,SIGNAL(clicked()),this,SLOT(selectbtnStartMqttClicked()));
    connect(btnThridExit, SIGNAL(clicked()), this, SLOT(btnThridExitEvent()));

    connect(checkbox[0],&QCheckBox::stateChanged,this,&Mqtt_client::checkboxStateChanged);
    connect(checkbox[1],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox1StateChanged);
    connect(checkbox[2],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox2StateChanged);
    connect(checkbox[3],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox3StateChanged);
    connect(checkbox[4],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox4StateChanged);
    connect(checkbox[5],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox5StateChanged);
    connect(checkbox[6],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox6StateChanged);
    connect(checkbox[7],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox7StateChanged);
    connect(checkbox[8],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox8StateChanged);
}

void Mqtt_client::setClientPort(int port)
{
    m_client->setPort(port);
}

void Mqtt_client::updateLogStateChange()
{

}

void Mqtt_client::connect_client()
{
    if (m_client->state() == QMqttClient::Disconnected)
    {
        lineEditHost->setEnabled(false);
        btnConnect->setText(tr("已连接"));
        btnConnect->setStyleSheet("background-color: red;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

        m_client->connectToHost();
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String("---Connect Success")
                + QLatin1Char('\n');
        plainTextEditLog->insertPlainText(content);
    }
    else
    {
        lineEditHost->setEnabled(true);
        btnConnect->setText(tr("连接"));
        btnConnect->setStyleSheet("background-color: rgb(78, 154, 6);color: rgb(255, 255, 255);border-radius:15px;font-size: 18px;");

        m_client->disconnectFromHost();
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String("---Disconnect Success")
                + QLatin1Char('\n');
        plainTextEditLog->insertPlainText(content);
        plainTextEditLog->clear();
    }
}

void Mqtt_client::Subscribe_topic()
{
    auto subscription = m_client->subscribe(lineEditTopicSub->text());
    if (!subscription)
    {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
    else
    {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String("---Subscribe_topic:  ")
                + lineEditTopicSub->text()
                + QLatin1Char('\n');
        plainTextEditLog->insertPlainText(content);
    }
}

void Mqtt_client::publish_msg()
{
    if (m_client->publish(lineEditTopicPub->text(), lineEditMsg->text().toUtf8()) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
}

void Mqtt_client::btnTextClearClicked()
{
   plainTextEditLog->clear();
}


void Mqtt_client::btnFirstExitEvent()
{
    m_client->disconnectFromHost();
    this->close();
    Showdata* showdata = new Showdata;
    showdata->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    showdata->show();
}

void Mqtt_client::btnSecondExitEvent()
{
    m_client->disconnectFromHost();
    this->close();
    Showdata* showdata = new Showdata;
    showdata->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    showdata->show();
}

void Mqtt_client::btnThridExitEvent()
{
    m_client->disconnectFromHost();
    this->close();
    Showdata* showdata = new Showdata;
    showdata->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    showdata->show();
}

void Mqtt_client::selectbtnclearClicked()
{
    for(int i = 0;i < 9;i++)
    {
        selectchSplineSeries[i]->clear();
    }
    selectmqtt_chart->axisX()->setMin(0);
    selectmqtt_chart->axisX()->setMax(MQTT_AXIS_MAX_X);
    selectmqtt_pointCount = 0;
}

void Mqtt_client::checkboxStateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[0]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[0]->setVisible(false);
        break;
    default:
        selectchSplineSeries[0]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox1StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[1]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[1]->setVisible(false);
        break;
    default:
        selectchSplineSeries[1]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox2StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[2]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[2]->setVisible(false);
        break;
    default:
        selectchSplineSeries[2]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox3StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[3]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[3]->setVisible(false);
        break;
    default:
        selectchSplineSeries[3]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox4StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[4]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[4]->setVisible(false);
        break;
    default:
        selectchSplineSeries[4]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox5StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[5]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[5]->setVisible(false);
        break;
    default:
        selectchSplineSeries[5]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox6StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[6]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[6]->setVisible(false);
        break;
    default:
        selectchSplineSeries[6]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox7StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[7]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[7]->setVisible(false);
        break;
    default:
        selectchSplineSeries[7]->setVisible(false);
        break;
    }
}

void Mqtt_client::checkbox8StateChanged(int state)
{
    switch(state)
    {
    case Qt::Checked:
        selectchSplineSeries[8]->setVisible(true);
        break;
    case Qt::Unchecked:
        selectchSplineSeries[8]->setVisible(false);
        break;
    default:
        selectchSplineSeries[8]->setVisible(false);
        break;
    }
}


void Mqtt_client::selectbtnStopMqttClicked()
{
    selectappendState = false;
}

void Mqtt_client::selectbtnStartMqttClicked()
{
    selectappendState = true;
}


