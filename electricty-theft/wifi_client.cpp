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
    listWidget->setGeometry(10,80,1000,504);
    //上一页按钮
    Wifi_btnpageup = new QPushButton(this);
    Wifi_btnpageup->setGeometry(320,10,50,50);
    Wifi_btnpageup->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/pageup.png);"
                "background-position: center;}"  // 图片居中
                 );

    //下一页按钮
    Wifi_btnpagedown = new QPushButton(this);
    Wifi_btnpagedown->setGeometry(400,10,50,50);
    Wifi_btnpagedown->setStyleSheet(
                "QPushButton{"
                "border: none solid none;"
                "border-radius: 15px;"
                "background-image: url(:/icons/pagedown.png);"
                "background-position: center;}"  // 图片居中
                 );

    //槽函数连接
    connect(Wifi_btnpageup, SIGNAL(clicked()), this, SLOT(Wifi_Btn_pageup()));
    connect(Wifi_btnpagedown, SIGNAL(clicked()), this, SLOT(Wifi_Btn_pagedown()));
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
        QRegularExpression signalRegex("Signal level=(-?\\d+)");
        QRegularExpression encryptionRegex("Encryption key:(on|off)");
        QRegularExpressionMatchIterator essidIterator  = essidRegex.globalMatch(wifiInfoContent);
        QRegularExpressionMatchIterator signalIterator = signalRegex.globalMatch(wifiInfoContent);
        QRegularExpressionMatchIterator encryptionIterator = encryptionRegex.globalMatch(wifiInfoContent);

        // 显示所有提取到的ESSID
        // 清空当前列表并重置页码
        currentPage = 0;
        essidList.clear();
        signalStrengthList.clear();
        encryptionList.clear();

        while (essidIterator.hasNext() && signalIterator.hasNext() && encryptionIterator.hasNext() )
        {
            QRegularExpressionMatch essidMatch  = essidIterator.next();
            QRegularExpressionMatch signalMatch = signalIterator.next();
            QRegularExpressionMatch encryptionMatch = encryptionIterator.next();
            QString essid = essidMatch.captured(1);
            QString signalStrength = signalMatch.captured(1);
            QString encryption = encryptionMatch.captured(1);
            essidList.append(essid);
            signalStrengthList.append(signalStrength);
            encryptionList.append(encryption);
        }

        PageConut = (essidList.size() % itemsPerPage) ? (essidList.size() / itemsPerPage) : (essidList.size() / itemsPerPage - 1);
        // 加载第一页
        loadNextPage(currentPage);
        connect(listWidget, &QListWidget::itemClicked, this, &Wifi_client::onItemClicked);
    }
    else
        qDebug()<< "scan failed";
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

void Wifi_client::loadNextPage(int page)
{
    int start = page * itemsPerPage;
    int end = qMin(start + itemsPerPage, essidList.size());
    // 清空当前列表项
    listWidget->clear();
    for (int i = start; i < end; ++i)
    {
        QListWidgetItem *pItem = new QListWidgetItem;
        pItem->setSizeHint(QSize(950, 50));  // 每次改变 Item 的高度
        pItem->setText(essidList[i]);

        // 创建自定义字体
        QFont font = pItem->font();
        font.setPointSize(16);  // 设置字体大小
        pItem->setFont(font);  // 应用字体到列表项

        //根据信号强度选择图标
        int signalStrength = signalStrengthList[i].toInt();
        QString signalIconPath;
        if (signalStrength <= 100 && signalStrength > 75 )
            signalIconPath = ":/icons/level4.png";
        else if (signalStrength <= 75 && signalStrength > 50 )
            signalIconPath = ":/icons/level3.png";
        else if (signalStrength <= 50 && signalStrength > 25)
            signalIconPath = ":/icons/level2.png";
        else
            signalIconPath = ":/icons/level1.png";

        // 选择加密状态图标
        QString encryptionIconPath;
        if (encryptionList[i] == "on")
            encryptionIconPath = ":/icons/ON.png";
        else
            encryptionIconPath = ":/icons/OFF.png";

        // 创建图标并设置大小
        QPixmap signalIcon(signalIconPath);
        signalIcon = signalIcon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 确保尺寸为50x50
        QPixmap encryptionIcon(encryptionIconPath);
        encryptionIcon = encryptionIcon.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation); // 确保尺寸为50x50

        QPixmap compositeIcon(100,50);
        compositeIcon.fill(Qt::transparent); // 设置背景透明
        QPainter painter(&compositeIcon);
        painter.drawPixmap(0, 0, signalIcon);
        painter.drawPixmap(signalIcon.width(), 0, encryptionIcon);

        QIcon icon(compositeIcon);
        pItem->setIcon(icon);
        listWidget->addItem(pItem);
    }
    listWidget->repaint(); // 触发重绘
}

void Wifi_client::Wifi_Btn_pageup()
{
    if(currentPage > 0)
    {
        currentPage--;
        loadNextPage(currentPage);
    }
}

void Wifi_client::Wifi_Btn_pagedown()
{
    if(currentPage < PageConut)
    {
        currentPage++;
        loadNextPage(currentPage);
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


