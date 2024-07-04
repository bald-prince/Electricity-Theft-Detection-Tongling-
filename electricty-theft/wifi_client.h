#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <QMainWindow>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QLocale>
#include <QTextStream>
#include <QThread>
#include <QLabel>
#include <QListWidget>
#include <QRegularExpression>
#include <QString>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QProcess>
#include <QScrollBar>
#include <QPixmap>
#include <QListView>
#include <QStandardItemModel>



class Wifi_client : public QMainWindow
{
    Q_OBJECT
public:
    explicit Wifi_client(QWidget *parent = nullptr);
    bool checkConnection();
private:
    QRadioButton *RadioButtonWifi;      //wifi开关按钮
    QPushButton *BtnExit;               //wifi退出按钮
    QPushButton *BtnRefresh;            //wifi刷新按钮
    QLabel* TitleLabel;                 //wifi标题
    QListWidget *listWidget;            //wifi显示列表
    QString wifiInfoContent;
    QInputDialog* inputDialog;          //密码输入框
    QProcess *ConnetWifi = nullptr;     //wifi连接进程
    QLabel* wifiState;                  //wifi状态显示
    QPushButton* Wifi_btnpagedown;       //下一页按钮
    QPushButton* Wifi_btnpageup;         //上一页按钮
    int PageConut = 0;                      //总页面数
    int currentPage = 0;
    const int itemsPerPage = 10;
    QStringList essidList;
    QStringList signalStrengthList;
    QStringList encryptionList;
    void loadNextPage(int page);

private slots:
    void OpenWifi(bool flag);           //wifi开关槽函数
    void ExitWifi();                    //退出wifi槽函数
    void RefreshWifi();                 //刷新列表槽函数
    void onItemClicked(QListWidgetItem *item);      //点击wifi槽函数
    void connectToWifi(const QString &essid, const QString &password);      //连接wifi槽函数

    void Wifi_Btn_pageup();                      //上一页槽函数
    void Wifi_Btn_pagedown();                     //跳转页面槽函数

signals:
};

#endif // WIFI_CLIENT_H
