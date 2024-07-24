#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QAction>
#include <QListView>
#include <QMessageBox>
#include "showdata.h"
#include <QPalette>
#include <QPixmap>
#include <QInputDialog>


class Login : public QWidget
{
    Q_OBJECT
public:
    explicit Login(QWidget *parent = nullptr);

private:
    QLineEdit *lineEditUser;            //用户名输入框
    QLineEdit *lineEditPassword;        //密码输入框
    QCheckBox *passwordBox;             //密码小眼睛
    QPushButton* btnLoginIn;            //登录按钮
    QPushButton* btnRegister;            //登录按钮
    QPushButton* btnExit;               //退出按钮
    QAction *actionUser;                //用户名输入框图标
    QAction *actionPassword;            //密码输入框图标
    QInputDialog* AdminInputDialog;          //密码输入框
    Showdata* showdata;                 //主界面


private slots:
    void checkbox_state_changed(int state);     //密码明文暗文切换槽函数
    void LogininClient();                       //登录槽函数
    void ExitClient();                          //退出槽函数
    void RegisterClient();                          //退出槽函数


signals:

};

#endif // LOGIN_H
