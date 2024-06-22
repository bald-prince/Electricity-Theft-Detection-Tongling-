#ifndef MYSQL_CLIENT_H
#define MYSQL_CLIENT_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QAction>
#include <QPlainTextEdit>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QTableWidget>
#include <QStandardItemModel>
#include <QTableView>
#include <QLabel>

class MySql_client : public QMainWindow
{
    Q_OBJECT
public:
    explicit MySql_client(QWidget *parent = nullptr);

private:
    QPushButton* btnConnectSql;     //mysql连接按钮
    QPushButton* btnExitSql;        //mysql退出按钮
    QLineEdit *lineEditHost;        //ip输入框
    QLineEdit *lineEditDatabase;    //数据库名输入框
    QLineEdit *lineEditUser;        //用户名输入框
    QLineEdit *lineEditPassword;    //密码输入框
    QLineEdit *lineEditPort;        //端口号输入框
    QCheckBox *passwordBox;         //密码小眼睛

    QAction *actionMysqlHost;       //ip图标
    QAction *actionMysqlDatabase;   //数据库名图标
    QAction *actionMysqlUser;       //用户名图标
    QAction *actionMysqlPassword;   //密码图标
    QAction *actionMysqlPort;       //端口号图标

    QPushButton* MysqldataExit;     //mysql登录页面退出按钮
    QPlainTextEdit* MysqlTextEdit;  //mysql显示页面退出按钮
    QTableView* tableView;          //mysql图标显示窗

    QStandardItemModel* model;

    QLineEdit *lineEditClientid;    //查询id输入框
    QLineEdit *lineEditPage;        //跳转页面输入框
    QPushButton* btnpagedown;       //下一页按钮
    QPushButton* btnpageup;         //上一页按钮
    QPushButton* pageset;           //跳转页面确认按钮
    QPushButton* btnFind;           //查询按钮
    QLabel* pagelabel;              //页面显示标签

    QList<QString> allpayloads;         //用于存储查询到的数据

    // 成员变量
    int currentPage = 0;                //页面
    const int rowsPerPage = 7;          //每一个显示的行数
    int PageConut = 0;                      //同页面数
    void updateQuery();                     //查询函数
    void updateTable(const int page);       //更新数据函数


private slots:
    void Btn_ConnectSqlDriver();        //登录mysql槽函数
    void Btn_ExitSqlDriver();               //退出槽函数
    void checkboxStateChanged(int state);   //密码明暗文显示槽函数
    void MysqldataExitEvent();              //退出槽函数
    void Btn_pagedown();                    //下一页槽函数
    void Btn_pageup();                      //上一页槽函数
    void Btn_pageset();                     //跳转页面槽函数
    void getAllPayload();                   //获取所有数据槽函数

};

#endif // MYSQL_CLIENT_H
