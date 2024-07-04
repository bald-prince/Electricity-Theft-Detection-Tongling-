#include "wifi_client.h"
#include "showdata.h"

Wifi_client::Wifi_client(QWidget *parent) : QMainWindow(parent)
{
    this->setGeometry(0, 0, 1024, 600);
    QPixmap backgroundPixmap(":/icons/desk.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, backgroundPixmap);
    this->setPalette(palette);
    this->setAutoFillBackground(true); // 确保窗口自动填充背景

    RadioButtonWifi = new QRadioButton(this);
    RadioButtonWifi->setGeometry(700,20,150,50);
    RadioButtonWifi->setText("OPEN");
    RadioButtonWifi->setChecked(true);
    RadioButtonWifi->setStyleSheet(
                "QRadioButton{spacing: 2px;color: white;border: none solid none;}"
                "QRadioButton::indicator:unchecked {image: url(:/icons/switch_off.png);}"
                "QRadioButton::indicator:checked {image: url(:/icons/switch_on.png);}"
                );

    BtnExit = new QPushButton(this);//退出按键
    BtnExit->setGeometry(960,20,50,50);

    BtnExit->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/wifi-exit.png);"
                "background-position: center;}"  // 图片居中
                 );

    BtnRefresh = new QPushButton(this);//退出按键
    BtnRefresh->setGeometry(880,20,50,50);

    BtnRefresh->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/refresh.png);"
                "background-position: center;}"  // 图片居中
                 );

    connect(BtnExit, SIGNAL(clicked()), this, SLOT(ExitWifi()));
    connect(BtnRefresh, SIGNAL(clicked()), this, SLOT(RefreshWifi()));
    connect(RadioButtonWifi, SIGNAL(toggled(bool)), this, SLOT(OpenWifi(bool)));

    TitleLabel = new QLabel(this);
    /* 使用资源里的文件时格式是 :+前缀+文件路径 */
    QPixmap pixmap(":icons/wifi-title.png");
    /* 标签大小为 452×132,根据图像的大小来设置 */
    TitleLabel->setGeometry(0, 0, 680, 70);
    /* 设置图像 */
    TitleLabel->setPixmap(pixmap);
    /* 开启允许缩放填充 */
    TitleLabel->setScaledContents(true);

    wifiState = new QLabel(this);
    //qDebug() << "Wi-Fi not connected!";
    QPixmap disconnect(":icons/wifi-disconnect.png");
    wifiState->setGeometry(620, 20, 50, 50);
    /* 设置图像 */
    wifiState->setPixmap(disconnect);
    /* 开启允许缩放填充 */
    wifiState->setScaledContents(true);

    listWidget = new QListWidget(this);
    listWidget->setGeometry(10,80,1000,500);
}

void Wifi_client::OpenWifi(bool flag)
{
    if(flag == true)
    {
        //qDebug() << "WIFI打开成功";
        RadioButtonWifi->setText("OPEN");
    }
    else
    {
        RadioButtonWifi->setText("CLOSE");
    }

}

void Wifi_client::ExitWifi()
{
    //qDebug()<<"EXIT";
    listWidget->clear();
    this->close();
    Showdata* showdata = new Showdata;
    showdata->setAttribute(Qt::WA_DeleteOnClose);     // 确保新窗口关闭时会被删除
    showdata->show();
}

void Wifi_client::RefreshWifi()
{
    if(RadioButtonWifi->isChecked())
    {
        system("ifconfig wlan0 down");
        system("rm ./wifi-info");
        system("ifconfig wlan0 up");
        //扫描所有的wifi信息，将名字存储到temp文件中
        system("iwlist wlan0 scan > ./wifi-info");
        qDebug()<< "scan finish";

        // 打开wifi-info文件
        QFile wifiInfoFile("./wifi-info");
        if (!wifiInfoFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "Failed to open wifi-info file";
            return;
        }
        wifiInfoContent = QString::fromUtf8(wifiInfoFile.readAll());
        wifiInfoFile.close();
        // 读取文件内容
        // 使用正则表达式提取所有的ESSID
        QRegularExpression essidRegex("ESSID:\"([^\"]*)\"");
        QRegularExpressionMatchIterator i = essidRegex.globalMatch(wifiInfoContent);

        // 显示所有提取到的ESSID
        listWidget->clear();
        while (i.hasNext())
        {
            QRegularExpressionMatch match = i.next();
            QString essid = match.captured(1);
            essidList.append(essid);
        }
        // 清空当前列表并重置页码
        listWidget->clear();
        currentPage = 0;

        // 加载第一页
        loadNextPage();
        connect(listWidget, &QListWidget::itemClicked, this, &Wifi_client::onItemClicked);
        // 连接滑动条的 valueChanged 信号到 loadNextPage 槽函数
        connect(listWidget->verticalScrollBar(), &QScrollBar::valueChanged, this, &Wifi_client::loadNextPage);
    }
    else
    {
        qDebug()<< "scan failed";
    }
}

void Wifi_client::loadNextPage()
{
    // 获取当前滑动条位置和最大值
    QScrollBar *scrollBar = listWidget->verticalScrollBar();
    if (scrollBar->value() != scrollBar->maximum())
        return; // 只有在滑动到底部时才加载下一页

    int start = currentPage * itemsPerPage;
    int end = qMin(start + itemsPerPage, essidList.size());

    for (int i = start; i < end; ++i)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setSizeHint(QSize(950, 45));  // 每次改变 Item 的高度
        pItem->setText(essidList[i]);
        // 创建自定义字体
        QFont font = pItem->font();
        font.setPointSize(14);  // 设置字体大小
        pItem->setFont(font);  // 应用字体到列表项
        listWidget->addItem(pItem);
    }

    currentPage++;
    listWidget->repaint(); // 触发重绘
}

void Wifi_client::onItemClicked(QListWidgetItem *item)
{
    //bool ok;
    QString essid = item->text();

    // 创建 QInputDialog 对象
    inputDialog = new QInputDialog(this);
    inputDialog->setWindowTitle(tr("输入密码"));
    inputDialog->setLabelText(tr("请输入密码:"));
    inputDialog->setTextEchoMode(QLineEdit::Password);
    inputDialog->setInputMode(QInputDialog::TextInput);
    inputDialog->setFixedSize(300, 100); // 设置对话框大小

    // 显示对话框并确保获取焦点
    inputDialog->activateWindow();
    inputDialog->raise();
    inputDialog->setFocus();
    inputDialog->show();

    if (inputDialog->exec() == QDialog::Accepted)
    {
        QString password = inputDialog->textValue();
        if (!password.isEmpty())
        {
            // 处理密码输入后的逻辑
            qDebug() << "ESSID:" << essid << ", Password:" << password;
            connectToWifi(essid,password);
            //通过system设置更新时间

        }
        else
            QMessageBox::warning(this, tr("警告"), tr("密码不能为空"));
    }
}

void Wifi_client::connectToWifi(const QString &essid, const QString &password)
{
    ConnetWifi = new QProcess(this);

    // 使用绝对路径构建脚本路径
    QString command = QString("./alientek_usb_wifi_setup.sh -m station -i %1 -p %2 -d wlan0 &").arg(essid).arg(password);

    qDebug() << command;
    // 执行命令
    ConnetWifi->start(command);

    if (!ConnetWifi->waitForStarted())
    {
        qDebug() << "Failed to start the process.";
        qDebug() << "Error: " << ConnetWifi->errorString();
        return;
    }

    ConnetWifi->waitForFinished();

    // 等待一段时间以确保连接完成
    QThread::sleep(5);
    bool connected = checkConnection();
    if (connected)
    {
        QPixmap connect(":icons/wifi-connect.png");
        wifiState->setGeometry(620, 20, 50, 50);
        wifiState->setPixmap(connect);/* 设置图像 */
        wifiState->setScaledContents(true);/* 开启允许缩放填充 */
        //更改时区
        system("ln -sf /usr/share/zoneinfo/Asia/Hong_Kong /etc/localtime");
        //自动对时
        system("ntpdate time.pool.aliyun.com");
    }
    else
    {
        QPixmap disconnect(":icons/wifi-disconnect.png");
        wifiState->setGeometry(620, 20, 50, 50);
        wifiState->setPixmap(disconnect);/* 设置图像 */
        wifiState->setScaledContents(true);/* 开启允许缩放填充 */
    }
}

bool Wifi_client::checkConnection()
{
    QProcess process;
    process.start("ip addr show wlan0");
    process.waitForFinished();
    QString output = QString(process.readAllStandardOutput());
    return output.contains("inet ");
}


