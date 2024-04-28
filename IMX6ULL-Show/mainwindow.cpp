#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    widget = new QWidget(this);
    this->setGeometry(0, 0, 1024, 600);
    this->setObjectName("project");
    widget->resize(this->size());
    this->windows_init();

    //通过system设置更新时间
    //更改时区
    system("ln -sf /usr/share/zoneinfo/Asia/Hong_Kong /etc/localtime");
    //自动对时
    system("ntpdate time.pool.aliyun.com");
}

MainWindow::~MainWindow()
{
}

void MainWindow::windows_init()
{
    QTextCodec*codec = QTextCodec::codecForName("utf-8");    //utf-8
    QTextCodec::setCodecForLocale(codec);
    //垂直布局实例化
    hBoxLayout = new QHBoxLayout();
    //堆栈部件实例化
    stackedWidget = new QStackedWidget();
    // 列表实例化
    listWidget = new QListWidget(this);
    listWidget->setStyleSheet("background-color: black;");

    hBoxLayout->setMargin(0);
    hBoxLayout->setSpacing(0);
    /*
    在创建WidgetItemData数组时即创建了每个窗口的实例。
    如果这些窗口在程序启动时并未立即用到，过早的实例化确实会消耗不必要的资源。
    考虑一种惰性加载（Lazy Loading）的方式，即在需要使用窗口时再将其实例化。
    这种方式需要配合工厂模式（Factory Pattern）使用，通过使用工厂方法在运行时动态创建相应的对象。
    */
    WidgetItemData items[] = {
        {"板级设备", []() -> QWidget * { return new Board; } },
        {"摄像头", []() -> QWidget * { return new Camera; } },
        {"MQTT", []() -> QWidget * { return new Mqtt_client; } },
    };

    for(auto & itemData : items)
    {
        QListWidgetItem *item = new QListWidgetItem(itemData.title);
        item->setTextColor(Qt::white);
        item->setTextAlignment(Qt::AlignCenter);
        item->setSizeHint(QSize(100, 60));
        listWidget->addItem(item);

        // 如果有对应的工厂函数，添加到stackedWidget中
        if (itemData.createInstance)
        {
            stackedWidget->addWidget(itemData.createInstance());
        }
    }

    // 将焦点设置为第一个项目
    if(listWidget->count() > 0)
    {
        QListWidgetItem* item1 = listWidget->item(0);
        listWidget->setCurrentItem(item1);
        listWidget->setItemSelected(item1, true);
        item1->setSelected(true);
        listWidget->setFocus();
    }

    //设置列表的最大宽度
    listWidget->setMaximumWidth(150);
    // 添加到水平布局
    hBoxLayout->addWidget(listWidget);
    hBoxLayout->addWidget(stackedWidget);

    // 将widget的布局设置成hboxLayout
    widget->setLayout(hBoxLayout);

    connect(listWidget, SIGNAL(currentRowChanged(int)),stackedWidget, SLOT(setCurrentIndex(int)));
}

