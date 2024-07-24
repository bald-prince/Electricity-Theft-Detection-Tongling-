#include "login.h"

Login::Login(QWidget *parent) : QWidget(parent)
{
    //背景显示
    QPixmap backgroundPixmap(":/icons/background.png");
    QPalette palette;
    palette.setBrush(QPalette::Background, backgroundPixmap);
    this->setPalette(palette);
    this->setAutoFillBackground(true); // 确保窗口自动填充背景

    //登录、退出按钮显示
    btnLoginIn = new QPushButton(tr("登录"),this);
    btnRegister = new QPushButton(tr("注册"),this);
    btnExit = new QPushButton(tr("退出"),this);
    //登录按钮初始化
    btnLoginIn->setGeometry(342,400,100,50);
    btnLoginIn->setFlat(false);
    btnLoginIn->setStyleSheet(
                "QPushButton{border: 1px solid gray;border-radius: 10px;background-color: rgb(97, 219, 232);color: rgb(255, 255, 255);}"
                     );
    //注册按钮初始化
    btnRegister->setGeometry(462,400,100,50);
    btnRegister->setFlat(false);
    btnRegister->setStyleSheet(
                "QPushButton{border: 1px solid gray;border-radius: 10px;background-color: rgb(97, 219, 232);color: rgb(255, 255, 255);}"
                     );


    //退出按钮初始化
    btnExit->setGeometry(582,400,100,50);
    btnExit->setStyleSheet(
                "QPushButton{border: 1px solid gray;border-radius: 10px;background-color: rgb(97, 219, 232);color: rgb(255, 255, 255);}"
                );
    btnExit->setFlat(false);
    //用户名图标初始化
    actionUser = new QAction(this);
    actionUser->setIcon(QIcon(":/icons/user.png"));
    //用户名输入框初始化
    lineEditUser = new QLineEdit(this);
    lineEditUser->setText("admin");
    lineEditUser->setPlaceholderText("User Name");
    lineEditUser->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditUser->setGeometry(362,170,300,50);
    lineEditUser->addAction(actionUser,QLineEdit::LeadingPosition);
    //密码图标
    actionPassword = new QAction(this);
    actionPassword->setIcon(QIcon(":/icons/password.png"));
    //密码输入框初始化
    lineEditPassword = new QLineEdit(this);
    lineEditPassword->setPlaceholderText("Password");
    lineEditPassword->setStyleSheet(
                "QLineEdit{border: 1px solid gray;border-radius: 10px;}"
                );
    lineEditPassword->setText("123456");
    lineEditPassword->setEchoMode(QLineEdit::Password);
    lineEditPassword->setGeometry(362,280,300,50);
    lineEditPassword->addAction(actionPassword,QLineEdit::LeadingPosition);
    //密码小眼睛初始化
    passwordBox = new QCheckBox(this);
    // 设置样式表（图片为眼睛样式）
    passwordBox->setStyleSheet(
                "QCheckBox {spacing: 1px;border: none solid none;}"
                "QCheckBox::indicator {width: 25px;height: 25px;border: none solid none;image: url(:/icons/not-display.png);}"
                "QCheckBox::indicator:checked {border: none solid none;image: url(:/icons/display.png);}"
                );
    connect(passwordBox, SIGNAL(stateChanged(int)),this, SLOT(checkbox_state_changed(int)));
    //设置控件的位置（需要根据QLineEdit的坐标进行调整）
    passwordBox->setGeometry(lineEditPassword->pos().x() + 260, lineEditPassword->pos().y() + 10,30,30);
    //登录、退出按钮槽函数连接
    connect(btnLoginIn, SIGNAL(clicked()), this, SLOT(LogininClient()));
    connect(btnExit, SIGNAL(clicked()), this, SLOT(ExitClient()));
    connect(btnRegister, SIGNAL(clicked()), this, SLOT(RegisterClient()));

}
//密码明暗文显示
void Login::checkbox_state_changed(int state)
{
    if(state)
        lineEditPassword->setEchoMode(QLineEdit::Normal);       //明文
    else
        lineEditPassword->setEchoMode(QLineEdit::Password);     //暗文
}

//登录
void Login::LogininClient()
{
    // 设置数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("login.db");
    db.open();

    QString username = lineEditUser->text();
    QString password = lineEditPassword->text();

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if (query.exec() && query.next())
    {
        this->close();
        showdata = new Showdata;
        showdata->setAttribute(Qt::WA_DeleteOnClose); // 确保新窗口关闭时会被删除
        showdata->show();
    }
    else
        QMessageBox::warning(this,tr("登录失败"),tr("用户名或密码输入错误！"),QMessageBox::Ok);
    // 在程序结束时关闭数据库连接
    db.close();
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);



//    //用户名和密码都正确
//    if(lineEditUser->text() == "admin" && lineEditPassword->text() == "123456")
//    {
//        //qDebug()<<"登录成功";
//        this->close();
//        showdata = new Showdata;
//        showdata->setAttribute(Qt::WA_DeleteOnClose); // 确保新窗口关闭时会被删除
//        showdata->show();
//    }
//    else
//        QMessageBox::warning(this,tr("登录失败"),tr("用户名或密码输入错误！"),QMessageBox::Ok);        //提示框
}

//退出
void Login::ExitClient()
{
    this->close();      //关闭当前页面
    QMessageBox::warning(this,tr("退出成功"),tr("欢迎下次使用，谢谢！"),QMessageBox::Ok);     //提示框
}

void Login::RegisterClient()
{
    // 设置数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("login.db");
    db.open();

    QString username = lineEditUser->text();
    QString password = lineEditPassword->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "用户名或密码不能为空", "请重新输入");
        return;
    }

    // 创建 QInputDialog 对象
    AdminInputDialog = new QInputDialog(this);
    AdminInputDialog->setWindowTitle(tr("输入密码"));
    AdminInputDialog->setLabelText(tr("请输入管理员密码:"));
    AdminInputDialog->setTextEchoMode(QLineEdit::Password);
    AdminInputDialog->setInputMode(QInputDialog::TextInput);
    AdminInputDialog->setFixedSize(300, 100); // 设置对话框大小

    // 显示对话框并确保获取焦点
    AdminInputDialog->activateWindow();
    AdminInputDialog->raise();
    AdminInputDialog->setFocus();
    AdminInputDialog->show();

    if (AdminInputDialog->exec() == QDialog::Accepted)
    {
        QString adminPassword = AdminInputDialog->textValue();
        if (adminPassword.isEmpty())
        {
            QMessageBox::warning(this, "注册失败", "需要管理员密码");
            return;
        }
        // 验证管理员密码
        const QString correctAdminPassword = "admin123";  // 假设管理员密码是 "admin123"
        if (adminPassword != correctAdminPassword)
        {
            QMessageBox::warning(this, "注册失败", "管理员密码错误");
            return;
        }
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (query.exec() && query.next())
    {
        QMessageBox::warning(this, "注册失败", "用户名已存在");
        return;
    }
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if (query.exec())
        QMessageBox::information(this, "注册成功", "注册成功");
    else
        QMessageBox::warning(this, "注册失败", "注册失败");
}

