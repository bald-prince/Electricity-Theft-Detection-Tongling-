#include "mqtt_client.h"

Mqtt_client::Mqtt_client(QWidget *parent) : QMainWindow(parent)
{
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

    QList <QString> strTabList;
    strTabList << "连接信息" << "文本显示" << "图表显示" << "选择显示";

    for(int i = 0;i < 4;i++)
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

    //    lineEditHost->setText("broker.emqx.io");
    //    lineEditPort->setText("1883");
    //    lineEditTopicSub->setText("ycl/subscribe_topic");

    //    lineEditTopicPub->setText("ycl/publish_topic");
    //    lineEditMsg->setText("hello_world");

    lineEditHost->setText("iot-060a8qgz.mqtt.iothub.aliyuncs.com");
    lineEditPort->setText("1883");
    lineEditTopicSub->setText("/k0hztVSi32P/mqtt_test/user/get");

    lineEditTopicPub->setText("ycl/publish_topic");
    lineEditMsg->setText("hello_world");
    //设置位置及大小
    labelHost->setGeometry(10,10,140,40);
    labelPort->setGeometry(450,10,140,40);
    lineEditHost->setGeometry(10,50,370,70);
    lineEditPort->setGeometry(450,50,370,70);

    btnConnect->setGeometry(50,450,100,50);

    labelTopicSub->setGeometry(10,140,140,40);
    labelQos->setGeometry(450,140,140,40);
    lineEditTopicSub->setGeometry(10,180,370,70);
    lineEditQos->setGeometry(450,180,370,70);

    btnSubscribe->setGeometry(362,450,100,50);

    labelTopicPub->setGeometry(10,280,140,40);
    labelMsg->setGeometry(450,280,140,40);
    lineEditTopicPub->setGeometry(10,320,370,70);
    lineEditMsg->setGeometry(450,320,370,70);

    btnPublish->setGeometry(674,450,100,50);

    //第二个页面
    secondpage = tabWidget->widget(1);
    plainTextEditLog = new QPlainTextEdit(secondpage);
    plainTextEditLog->setGeometry(0,0,824,580);

    btnTextClear = new QPushButton(tr("清除"),secondpage);
    btnTextClear->setGeometry(690,10,100,40);
    btnTextClear->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    m_client = new QMqttClient(this);
    m_client->setHostname(lineEditHost->text());
    m_client->setPort(lineEditPort->text().toUInt());
    m_client->setClientId("k0hztVSi32P.mqtt_test|securemode=2,signmethod=hmacsha256,timestamp=1709770574822|");
    m_client->setUsername("mqtt_test&k0hztVSi32P");
    m_client->setPassword("0d739648e91d7241ee346574768f80d2e3ea508782c9031c72d6206768ecee05");

    connect(btnTextClear,SIGNAL(clicked()),this,SLOT(btnTextClearClicked()));
    connect(m_client, &QMqttClient::stateChanged, this, &Mqtt_client::updateLogStateChange);

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
        /* QString stringData = QString::fromUtf8(message);
        // 将 JSON 字符串转换为 QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(stringData.toUtf8());
        // 检查 JSON 是否有效
        if (jsonDoc.isNull())
            return;
        // 获取 JSON 对象
        QJsonObject jsonObj = jsonDoc.object();
        // 获取 down_data 数组
        QJsonValue downDataValue = jsonObj.value("down_data");
        if (!downDataValue.isArray())
            return;
        // 获取 down_data 数组中的第二个元素，即Current1-Current8数组
        QJsonArray currentArray = downDataValue.toArray().at(1).toObject().value("Current1-Current8").toArray();
        // 提取数组中的值
        QVector<double> currentValues;
        for (const QJsonValue& value : currentArray)
        {
            if (value.isDouble())
                currentValues.append(value.toDouble());
        }*/
        QString stringData = QString::fromUtf8(message);
        // 将 JSON 字符串转换为 QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(stringData.toUtf8());
        // 检查 JSON 是否有效
        if (jsonDoc.isNull())
            return;
        // 获取 JSON 对象
        QJsonObject jsonObj = jsonDoc.object();
        // 获取"value"字段对应的QJsonArray
        QJsonArray valueArray = jsonObj["value"].toArray();
        // 查找包含 "Current1-Current8" 的对象
        QJsonObject currentObject;
        for (const auto& obj : valueArray)
        {
            if (obj.isObject() && obj.toObject().contains("Current1-Current8"))
            {
                currentObject = obj.toObject();
                break;
            }
        }
        // 提取 "Current1-Current8" 对应的QJsonArray
        QJsonArray currentArray = currentObject["Current1-Current8"].toArray();
        // 将数据存储到数组中

        for (const auto& value : currentArray)
            currentValues.append(value.toDouble());

        //画曲线代码
        if(mqtt_pointCount > MQTT_AXIS_MAX_X)
        {
            chSplineSeries[0]->remove(0);
            mqtt_axisX->setMin(mqtt_pointCount - MQTT_AXIS_MAX_X);
            mqtt_axisX->setMax(mqtt_pointCount);                    // 更新X轴范围
        }

        if(appendState)
        {
            for(int i = 0;i < 8; i++)
                chSplineSeries[i]->append(QPointF(mqtt_pointCount, currentValues.at(i)));  // 更新显示
            mqtt_pointCount++;
        }

        //画曲线代码
        if(selectmqtt_pointCount > MQTT_AXIS_MAX_X)
        {
            selectchSplineSeries[0]->remove(0);
            mqtt_axisX->setMin(selectmqtt_pointCount - MQTT_AXIS_MAX_X);
            mqtt_axisX->setMax(selectmqtt_pointCount);                    // 更新X轴范围
        }

        if(selectappendState)
        {
            for(int i = 0;i < 8; i++)
                selectchSplineSeries[i]->append(QPointF(selectmqtt_pointCount, currentValues.at(i)));  // 更新显示
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
    mqtt_chart_init();


    //第四个界面
    fourthpage = tabWidget->widget(3);
    mqtt_chart_init_select();


    hBoxLayout->addWidget(tabWidget);
    widget->setLayout(hBoxLayout);
}

void Mqtt_client::mqtt_chart_init()
{
    //图表显示
    //1.设置坐标轴
    btnclear = new QPushButton(tr("清除"),thirdpage);
    btnclear->setGeometry(200,10,80,40);

    btnStopMqtt = new QPushButton(tr("暂停"),thirdpage);
    btnStopMqtt->setGeometry(100,10,80,40);

    btnStartMqtt = new QPushButton(tr("开始"),thirdpage);
    btnStartMqtt->setGeometry(10,10,80,40);

    mqtt_axisX = new QValueAxis();
    mqtt_axisY = new QValueAxis();
    mqtt_axisX->setLabelsColor(Qt::white);
    mqtt_axisY->setLabelsColor(Qt::white);
    mqtt_axisX->setLinePenColor(Qt::white);
    mqtt_axisY->setLinePenColor(Qt::white);
    mqtt_axisX->setTitleBrush(QBrush(Qt::white));
    mqtt_axisY->setTitleBrush(QBrush(Qt::white));

    mqtt_axisX->setTitleText("X-label");
    mqtt_axisY->setTitleText("电流/A");
    mqtt_axisX->setMin(0);
    mqtt_axisY->setMax(-2);
    mqtt_axisX->setMax(MQTT_AXIS_MAX_X);
    mqtt_axisY->setMax(MQTT_AXIS_MAX_Y);

    QList<QColor> colors;
    colors << "#47A4E9" << "#00B17D" << "#D64D54" << "#DEAF39" << "#A279C5" << "#009679" << "#0B658B" << "#8B008B";

    for(int i = 0;i < 8;i++)
    {
        QString name = "通道" + QString::number(i+1);
        chSplineSeries[i] = new QSplineSeries();
        QPen pen(colors.at(i));
        pen.setWidth(3);
        chSplineSeries[i]->setPen(pen);
        chSplineSeries[i]->setPointsVisible(true);
        chSplineSeries[i]->setName(name);
    }

    mqtt_chart = new QChart();                                        // 创建图表对象
    mqtt_chartView = new QChartView(thirdpage);
    mqtt_chartView->setGeometry(0,50,874,530);
    mqtt_chart->setBackgroundBrush(QBrush(Qt::black));
    mqtt_chart->setTitleBrush(QBrush(Qt::white));
    mqtt_chart->legend()->setBrush(QBrush(Qt::white));
    mqtt_chart->legend()->setLabelColor(Qt::white);

    mqtt_chart->addAxis(mqtt_axisY, Qt::AlignLeft);                      // 将X轴添加到图表上
    mqtt_chart->addAxis(mqtt_axisX, Qt::AlignBottom);                    // 将Y轴添加到图表上

    for(int i =0;i < 8;i++)
    {
        mqtt_chart->addSeries(chSplineSeries[i]);
        chSplineSeries[i]->attachAxis(mqtt_axisX);  //als曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
        chSplineSeries[i]->attachAxis(mqtt_axisY);  // als曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后
    }

    mqtt_chart->setAnimationOptions(QChart::SeriesAnimations);        // 动画：能使曲线绘制显示的更平滑，过渡效果更好看

    mqtt_chartView->setChart(mqtt_chart);
    mqtt_chartView->setRenderHint(QPainter::Antialiasing);

    connect(btnclear,SIGNAL(clicked()),this,SLOT(btnclearClicked()));
    connect(btnStopMqtt,SIGNAL(clicked()),this,SLOT(btnStopMqttClicked()));
    connect(btnStartMqtt,SIGNAL(clicked()),this,SLOT(btnStartMqttClicked()));
}

void Mqtt_client::mqtt_chart_init_select()
{
    selectbtnclear = new QPushButton(tr("清除"),fourthpage);
    selectbtnclear->setGeometry(800,110,60,30);

    selectbtnStopMqtt = new QPushButton(tr("暂停"),fourthpage);
    selectbtnStopMqtt->setGeometry(800,60,60,30);

    selectbtnStartMqtt = new QPushButton(tr("开始"),fourthpage);
    selectbtnStartMqtt->setGeometry(800,10,60,30);

    QString nameseed = "CH";
    for(int i = 0;i < 8;i++)
    {
        checkbox[i] = new QCheckBox(tr("%1%2").arg(nameseed).arg(i+1),fourthpage);
        checkbox[i]->setGeometry(800,150 + 45 * i,70,50);
        checkbox[i]->setCheckState(Qt::Unchecked);
        //selectchSplineSeries[i]->setVisible(false);
    }

    mqtt_axisX = new QValueAxis();
    mqtt_axisY = new QValueAxis();
    mqtt_axisX->setLabelsColor(Qt::white);
    mqtt_axisY->setLabelsColor(Qt::white);
    mqtt_axisX->setLinePenColor(Qt::white);
    mqtt_axisY->setLinePenColor(Qt::white);
    mqtt_axisX->setTitleBrush(QBrush(Qt::white));
    mqtt_axisY->setTitleBrush(QBrush(Qt::white));

    mqtt_axisX->setTitleText("X-label");
    mqtt_axisY->setTitleText("电流/A");
    mqtt_axisX->setMin(0);
    mqtt_axisY->setMax(-2);
    mqtt_axisX->setMax(MQTT_AXIS_MAX_X);
    mqtt_axisY->setMax(MQTT_AXIS_MAX_Y);

    QList<QColor> colors;
    colors << "#47A4E9" << "#00B17D" << "#D64D54" << "#DEAF39" << "#A279C5" << "#009679" << "#0B658B" << "#8B008B";

    for(int i = 0;i < 8;i++)
    {
        QString name = "通道" + QString::number(i+1);
        selectchSplineSeries[i] = new QSplineSeries();
        QPen pen(colors.at(i));
        pen.setWidth(3);
        selectchSplineSeries[i]->setPen(pen);
        selectchSplineSeries[i]->setPointsVisible(true);
        selectchSplineSeries[i]->setName(name);
        selectchSplineSeries[i]->setVisible(false);
    }

    selectmqtt_chart = new QChart();                                        // 创建图表对象
    selectmqtt_chartView = new QChartView(fourthpage);
    selectmqtt_chartView->setGeometry(0,0,800,560);
    selectmqtt_chart->setBackgroundBrush(QBrush(Qt::black));
    selectmqtt_chart->setTitleBrush(QBrush(Qt::white));
    selectmqtt_chart->legend()->setBrush(QBrush(Qt::white));
    selectmqtt_chart->legend()->setLabelColor(Qt::white);

    selectmqtt_chart->addAxis(mqtt_axisY, Qt::AlignLeft);                      // 将X轴添加到图表上
    selectmqtt_chart->addAxis(mqtt_axisX, Qt::AlignBottom);                    // 将Y轴添加到图表上

    for(int i =0;i < 8;i++)
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

    connect(checkbox[0],&QCheckBox::stateChanged,this,&Mqtt_client::checkboxStateChanged);
    connect(checkbox[1],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox1StateChanged);
    connect(checkbox[2],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox2StateChanged);
    connect(checkbox[3],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox3StateChanged);
    connect(checkbox[4],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox4StateChanged);
    connect(checkbox[5],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox5StateChanged);
    connect(checkbox[6],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox6StateChanged);
    connect(checkbox[7],&QCheckBox::stateChanged,this,&Mqtt_client::checkbox7StateChanged);
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

void Mqtt_client::btnclearClicked()
{
    for(int i = 0;i < 8;i++)
    {
        chSplineSeries[i]->clear();
    }
    mqtt_chart->axisX()->setMin(0);
    mqtt_chart->axisX()->setMax(MQTT_AXIS_MAX_X);
    mqtt_pointCount = 0;
}

void Mqtt_client::btnTextClearClicked()
{
    plainTextEditLog->clear();
}

void Mqtt_client::btnStopMqttClicked()
{
    appendState = false;
}

void Mqtt_client::btnStartMqttClicked()
{
    appendState = true;
}


void Mqtt_client::selectbtnclearClicked()
{
    for(int i = 0;i < 8;i++)
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


void Mqtt_client::selectbtnStopMqttClicked()
{
    selectappendState = false;
}

void Mqtt_client::selectbtnStartMqttClicked()
{
    selectappendState = true;
}

