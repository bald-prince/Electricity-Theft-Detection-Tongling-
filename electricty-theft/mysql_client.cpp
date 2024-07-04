#include "mysql_client.h"
#include "showdata.h"

MySql_client::MySql_client(QWidget *parent) : QMainWindow(parent)
{
    this->setGeometry(0, 0, 1024, 600);
    QPixmap backgroundPixmap(":/icons/desk.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, backgroundPixmap);
    this->setPalette(palette);
    this->setAutoFillBackground(true); // 确保窗口自动填充背景
    //ip图标
    actionMysqlHost = new QAction(this);
    actionMysqlHost->setIcon(QIcon(":/icons/mysql-host.png"));
    //ip输入框初始化
    lineEditHost = new QLineEdit(this);
    lineEditHost->setPlaceholderText("Host IP");
    lineEditHost->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditHost->setText("116.198.227.107");
    lineEditHost->setGeometry(362,50,300,50);
    lineEditHost->addAction(actionMysqlHost,QLineEdit::LeadingPosition);
    //数据库名图标
    actionMysqlDatabase = new QAction(this);
    actionMysqlDatabase->setIcon(QIcon(":/icons/mysql-database.png"));
    //数据库名输入框初始化
    lineEditDatabase = new QLineEdit(this);
    lineEditDatabase->setPlaceholderText("Database Name");
    lineEditDatabase->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditDatabase->setText("emqx_data");
    lineEditDatabase->setGeometry(362,130,300,50);
    lineEditDatabase->addAction(actionMysqlDatabase,QLineEdit::LeadingPosition);
    //用户名图标
    actionMysqlUser = new QAction(this);
    actionMysqlUser->setIcon(QIcon(":/icons/mysql-user.png"));
    //用户名输入框初始化
    lineEditUser = new QLineEdit(this);
    lineEditUser->setPlaceholderText("User Name");
    lineEditUser->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditUser->setText("root");
    lineEditUser->setGeometry(362,210,300,50);
    lineEditUser->addAction(actionMysqlUser,QLineEdit::LeadingPosition);
    //密码图标
    actionMysqlPassword = new QAction(this);
    actionMysqlPassword->setIcon(QIcon(":/icons/mysql-password.png"));
    //密码输入框初始化
    lineEditPassword = new QLineEdit(this);
    lineEditPassword->setPlaceholderText("Password");
    lineEditPassword->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditPassword->setText("smart_luggage907!");
    lineEditPassword->setGeometry(362,290,300,50);
    lineEditPassword->setEchoMode(QLineEdit::Password);
    lineEditPassword->addAction(actionMysqlPassword,QLineEdit::LeadingPosition);
    //密码明、暗文切换按钮
    passwordBox = new QCheckBox(this);
    // 设置样式表（图片为眼睛样式）
    passwordBox->setStyleSheet(
                "QCheckBox {spacing: 5px;border: none;background-color: transparent;}"
                "QCheckBox::indicator {width: 20px;height: 20px;border: none;image: url(:/icons/not-display.png);}"
                "QCheckBox::indicator:checked {image: url(:/icons/display.png);}"
                );
    connect(passwordBox, SIGNAL(stateChanged(int)),this, SLOT(checkboxStateChanged(int)));
    //设置控件的位置（需要根据QLineEdit的坐标进行调整）
    passwordBox->setGeometry(lineEditPassword->pos().x() + 275,lineEditPassword->pos().y() + 15,20,20);
    //端口号图标
    actionMysqlPort = new QAction(this);
    actionMysqlPort->setIcon(QIcon(":/icons/mysql-port.png"));
    //端口号输入框初始化
    lineEditPort = new QLineEdit(this);
    lineEditPort->setPlaceholderText("Port");
    lineEditPort->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 8px;}"
                );
    lineEditPort->setGeometry(362,370,300,50);
    lineEditPort->setText("9306");
    lineEditPort->addAction(actionMysqlPort,QLineEdit::LeadingPosition);

    //连接驱动按钮
    btnConnectSql = new QPushButton(tr("连接驱动"),this);
    btnConnectSql->setGeometry(362,450,100,50);
    connect(btnConnectSql, SIGNAL(clicked()), this, SLOT(Btn_ConnectSqlDriver()));
    btnConnectSql->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

    btnExitSql = new QPushButton(tr("退出"),this);
    btnExitSql->setGeometry(562,450,100,50);
    connect(btnExitSql, SIGNAL(clicked()), this, SLOT(Btn_ExitSqlDriver()));
    btnExitSql->setStyleSheet("background-color: black;color: rgb(255, 255, 255); border-radius:15px;font-size: 18px;");

}

void MySql_client::Btn_ConnectSqlDriver()
{
    QSqlDatabase my_db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        my_db = QSqlDatabase::database("qt_sql_default_connection");
    else
        my_db = QSqlDatabase::addDatabase("QMYSQL");

    my_db.setHostName(lineEditHost->text());                    //116.198.227.107
    my_db.setDatabaseName(lineEditDatabase->text());            //emqx_data
    my_db.setUserName(lineEditUser->text());                    //root
    my_db.setPassword(lineEditPassword->text());                //smart_luggage907!
    my_db.setPort(lineEditPort->text().toInt());                //9306

    bool ok = my_db.open();
    if(ok)
    {
        //qDebug() << "连接成功";
        lineEditHost->close();
        lineEditDatabase->close();
        lineEditPassword->close();
        lineEditPort->close();
        lineEditUser->close();
        btnConnectSql->close();
        btnExitSql->close();
        //退出按钮初始化
        MysqldataExit = new QPushButton(this);
        MysqldataExit->setGeometry(960,10,50,50);
        MysqldataExit->setStyleSheet(
                    "QPushButton{"
                    "border: none solid none;"
                    "border-radius: 15px;"
                    "background-image: url(:/icons/wifi-exit.png);"
                    "background-position: center;}"  // 图片居中
                     );
        MysqldataExit->show();
        connect(MysqldataExit, SIGNAL(clicked()), this, SLOT(MysqldataExitEvent()));

//        MysqlTextEdit = new QPlainTextEdit(this);
//        MysqlTextEdit->setGeometry(10,80,1000,500);
//        MysqlTextEdit->setStyleSheet("QPlainTextEdit{border: 1px solid gray;border-radius: 10px;}");
//        MysqlTextEdit->show();
        //查询输入框初始化
        lineEditClientid = new QLineEdit(this);
        lineEditClientid->setGeometry(10,10,200,50);
        lineEditClientid->setStyleSheet("QLineEdit{border: 1px solid gray;border-radius: 15px;}");
        lineEditClientid->show();
        //跳转页面输入框初始化
        lineEditPage = new QLineEdit(this);
        lineEditPage->setGeometry(480,10,150,50);
        lineEditPage->setPlaceholderText("跳转到指定页");
        lineEditPage->setStyleSheet("QLineEdit{border: 1px solid gray;border-radius: 15px;}");
        lineEditPage->show();

        model = new QStandardItemModel();
        tableView = new QTableView(this);
        tableView->setGeometry(0,80,1024,500);
        tableView->setStyleSheet("QTableView{border: 1px solid gray;border-radius: 15px;}");
        // 设置表格列数和表头
        model->setColumnCount(2);
        QStringList headers;
        headers << "ClientId" << "Payload";
        model->setHorizontalHeaderLabels(headers);
        tableView->setModel(model);

        // 设置表格的宽度
        tableView->setColumnWidth(0, 124);
        tableView->setColumnWidth(1, 900);
        tableView->show();
        //上一页按钮
        btnpageup = new QPushButton(this);
        btnpageup->setGeometry(320,10,50,50);
        btnpageup->setStyleSheet(
                    "QPushButton{"
                    "border: none solid none;"
                    "border-radius: 15px;"
                    "background-image: url(:/icons/pageup.png);"
                    "background-position: center;}"  // 图片居中
                     );
        btnpageup->show();
        //查询按钮
        btnFind = new QPushButton(this);
        btnFind->setGeometry(240,10,50,50);
        btnFind->setStyleSheet(
                    "QPushButton{"
                    "border: none solid none;"
                    "border-radius: 15px;"
                    "background-image: url(:/icons/query.png);"
                    "background-position: center;}"  // 图片居中
                     );
        btnFind->show();
        //下一页按钮
        btnpagedown = new QPushButton(this);
        btnpagedown->setGeometry(400,10,50,50);
        btnpagedown->setStyleSheet(
                    "QPushButton{"
                    "border: none solid none;"
                    "border-radius: 15px;"
                    "background-image: url(:/icons/pagedown.png);"
                    "background-position: center;}"  // 图片居中
                     );
        btnpagedown->show();
        //跳转页面初始化按钮
        pageset = new QPushButton(this);
        pageset->setGeometry(640,10,50,50);
        pageset->setStyleSheet(
                    "QPushButton{"
                    "border: none solid none;"
                    "border-radius: 15px;"
                    "background-image: url(:/icons/set.png);"
                    "background-position: center;}"  // 图片居中
                     );
        pageset->show();
        //槽函数连接
        connect(btnpageup, SIGNAL(clicked()), this, SLOT(Btn_pageup()));
        connect(btnpagedown, SIGNAL(clicked()), this, SLOT(Btn_pagedown()));
        connect(pageset, SIGNAL(clicked()), this, SLOT(Btn_pageset()));
        connect(btnFind, SIGNAL(clicked()), this, SLOT(getAllPayload()));
        //页码显示标签
        pagelabel = new QLabel(this);
        pagelabel->setGeometry(360,60,130,20);
        pagelabel->setText(QString("第 0/%1 页").arg(PageConut));
        pagelabel->show();
    }
    else
        QMessageBox::warning(this,tr("登录失败"),tr("登录失败，请重新登录！"),QMessageBox::Ok);
}

//退出mysql登录界面
//关闭当前页面并打开主界面
void MySql_client::Btn_ExitSqlDriver()
{
    this->close();
    Showdata *showdata = new Showdata;
    showdata->setAttribute(Qt::WA_DeleteOnClose);          // 确保新窗口关闭时会被删除
    showdata->show();
}

//密码明暗文切换
void MySql_client::checkboxStateChanged(int state)
{
    if(state)
        lineEditPassword->setEchoMode(QLineEdit::Normal);
    else
        lineEditPassword->setEchoMode(QLineEdit::Password);
}
//登录界面退出
void MySql_client::MysqldataExitEvent()
{
    this->close();
    Showdata *showdata = new Showdata;
    showdata->setAttribute(Qt::WA_DeleteOnClose);          // 确保新窗口关闭时会被删除
    showdata->show();
}

//下一页
void MySql_client::Btn_pagedown()
{
    if(currentPage < PageConut)
    {
        currentPage++;
        pagelabel->setText(QString("第 %1/%2 页").arg(currentPage).arg(PageConut));
        //updateQuery();
        updateTable(currentPage);
    }
}
//上一页
void MySql_client::Btn_pageup()
{
    if (currentPage > 0)
    {
        currentPage--;
        pagelabel->setText(QString("第 %1/%2 页").arg(currentPage).arg(PageConut));
        //updateQuery();
        updateTable(currentPage);
    }
}

//跳转页面
void MySql_client::Btn_pageset()
{
    if(lineEditPage->text().toInt() > PageConut)
        QMessageBox::warning(this,tr("查询失败"),tr("查询失败，超出最大页数！"),QMessageBox::Ok);
    else
    {
        currentPage = lineEditPage->text().toInt();
        pagelabel->setText(QString("第 %1/%2 页").arg(currentPage).arg(PageConut));
        updateTable(currentPage);
        lineEditPage->clear();
    }
}

//第一种方式：一次只查询指定行数
void MySql_client::updateQuery()
{
    QSqlQuery query;
    query.prepare("SELECT payload FROM emqx_messages WHERE clientid = :clientid LIMIT :limit OFFSET :offset");
    query.bindValue(":clientid", lineEditClientid->text());
    query.bindValue(":limit", rowsPerPage);
    query.bindValue(":offset", currentPage * rowsPerPage);

    if (query.exec())
    {
        model->removeRows(0, model->rowCount());  // 清空之前的查询结果
        int rowNumber = 0;  // 行号从0开始
        while (query.next())
        {
            QString payload = query.value(0).toString();
            QList<QStandardItem *> items;

            QStandardItem *clientidItem = new QStandardItem(lineEditClientid->text());
            QStandardItem *payloadItem = new QStandardItem(payload);

            // 设置不可编辑
            clientidItem->setFlags(clientidItem->flags() & ~Qt::ItemIsEditable);
            payloadItem->setFlags(payloadItem->flags() & ~Qt::ItemIsEditable);

            items.append(clientidItem);
            items.append(payloadItem);

            model->insertRow(rowNumber, items);

            // 设置当前行的高度
            tableView->setRowHeight(rowNumber, 60);  // 40 是高度的像素值，可以根据需要调整

            rowNumber++;
        }
    }
    else
        QMessageBox::warning(this, tr("查询失败"), tr("不存在的Clientid！"), QMessageBox::Ok);
}

//第二种方式：先全部查询出来并存储，然后每次只显示指定行数
void MySql_client::updateTable(const int page)
{
    model->removeRows(0, model->rowCount());  // 清空之前的查询结果
    int start = page * rowsPerPage;
    int end = qMin(start + rowsPerPage, allpayloads.size());
    for (int i = start; i < end; ++i)
    {
        const auto &item = allpayloads.at(i);
        //qDebug() << allpayloads.at(i);
        QList<QStandardItem *> items;
        QStandardItem *clientidItem = new QStandardItem(lineEditClientid->text());
        QStandardItem *payloadItem = new QStandardItem(item);

        // 设置不可编辑
        clientidItem->setFlags(clientidItem->flags() & ~Qt::ItemIsEditable);
        payloadItem->setFlags(payloadItem->flags() & ~Qt::ItemIsEditable);

        items.append(clientidItem);
        items.append(payloadItem);

        model->insertRow(i - start, items);

        // 设置当前行的高度
        tableView->setRowHeight(i - start, 62);
    }
}

void MySql_client::getAllPayload()
{
    allpayloads.clear();
    QSqlQuery query;
    query.prepare("SELECT payload FROM emqx_messages WHERE clientid = :clientid");
    query.bindValue(":clientid", lineEditClientid->text());
    if (query.exec())
    {
        int count = 0;
        while (query.next())
        {
            QString payload = query.value(0).toString();
            allpayloads.append(payload);            //添加到allpayloads
            count++;
        }
        int temp = count % rowsPerPage;
        PageConut = (temp == 0)?(count / rowsPerPage) : (count / rowsPerPage + 1);
        pagelabel->setText(QString("第 0/%1 页").arg(PageConut));
    }
}
