#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>
#include <QLabel>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 设置主窗口背景颜色
    this->setStyleSheet("background-color: #3c3f41;");

    /*****关于菜单*****/

    // 创建菜单
    QMenu *menu = menuBar()->addMenu("帮助");
    menu->setStyleSheet("QMenu { color: #a9b7c6; }");

    // 创建动作
    auto *aboutAction = new QAction("关于", this);

    // 添加动作到菜单
    menu->addAction(aboutAction);

    // 连接信号和槽
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);

    /*****创建 MQTT Broker 标签*****/

    auto *brokerLabel = new QLabel("MQTT Broker:", this); // 创建 MQTT Broker 标签
    brokerLabel->setStyleSheet("QLabel { color : #a9b7c6; }");
    brokerLabel->setGeometry(QRect(QPoint(50, 50), QSize(100, 20)));
    brokerLineEdit = new QLineEdit(this); // 创建 MQTT Broker 输入框
    brokerLineEdit->setGeometry(QRect(QPoint(160, 50), QSize(200, 20)));

    /*****创建用户名标签*****/

    auto *usernameLabel = new QLabel("用户名:", this); // 创建用户名标签
    usernameLabel->setStyleSheet("QLabel { color : #a9b7c6; }");
    usernameLabel->setGeometry(QRect(QPoint(50, 80), QSize(100, 20)));
    usernameLineEdit = new QLineEdit(this); // 创建用户名输入框
    usernameLineEdit->setGeometry(QRect(QPoint(160, 80), QSize(200, 20)));


    /*****创建密码标签*****/

    auto *passwordLabel = new QLabel("密码:", this); // 创建密码标签
    passwordLabel->setStyleSheet("QLabel { color : #a9b7c6; }");
    passwordLabel->setGeometry(QRect(QPoint(50, 110), QSize(100, 20)));
    passwordLineEdit = new QLineEdit(this); // 创建密码输入框
    passwordLineEdit->setGeometry(QRect(QPoint(160, 110), QSize(200, 20)));
    passwordLineEdit->setEchoMode(QLineEdit::Password); // 设置密码输入框的回显模式为密码模式

    // 设置输入框的背景颜色和文字颜色
    QString lineEditStyleSheet = "QLineEdit { background-color: #2b2b2b; color: #6a9dc5; border: 1px solid #2b2b2b; }";
    brokerLineEdit->setStyleSheet(lineEditStyleSheet);
    usernameLineEdit->setStyleSheet(lineEditStyleSheet);
    passwordLineEdit->setStyleSheet(lineEditStyleSheet);

    /*****创建主题数量标签*****/

    auto *topicCountLabel = new QLabel("Topic数量:", this); // 创建主题数量标签
    topicCountLabel->setStyleSheet("QLabel { color : #a9b7c6; }");
    topicCountLabel->setGeometry(QRect(QPoint(50, 140), QSize(100, 20)));

    /*****创建主题数量选择框*****/

    topicCountComboBox = new QComboBox(this); // 创建主题数量选择框
    topicCountComboBox->setGeometry(QRect(QPoint(160, 140), QSize(200, 20)));
    for (int i = 1; i <= 10; ++i) {
        topicCountComboBox->addItem(QString::number(i), i);
    }
    topicCountComboBox->setStyleSheet("QComboBox { color: #a9b7c6; }"); // 设置选择框的文字颜色
    connect(topicCountComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTopicCountChanged); // 连接主题数量选择框的选择改变事件到相应的处理函数

    /*****创建主题输入框的滚动区域*****/

    topicScrollArea = new QScrollArea(this); // 创建主题输入框的滚动区域
    topicScrollArea->setStyleSheet("QScrollArea { border: 1px solid #3c3f41; }");
    topicScrollArea->setGeometry(QRect(QPoint(50, 170), QSize(310, 300)));
    auto *scrollAreaContent = new QWidget(topicScrollArea);
    topicScrollArea->setWidgetResizable(true);
    topicScrollArea->setWidget(scrollAreaContent);

    /*****创建主题输入框的布局*****/

    topicLayout = new QVBoxLayout(scrollAreaContent); // 创建主题输入框的布局

    /*****创建订阅按钮*****/

    submitButton = new QPushButton("订阅", this); // 创建提交按钮
    // 设置QPushButton的圆角卡片效果
    submitButton->setStyleSheet(
            "QPushButton {"
            "border: 1px solid #353740;"
            "border-radius: 10px;"
            "background-color: #569142;"
            "color: #a9b7c6;"
            "}"
            "QPushButton:pressed {"
            "background-color: #353740;"
            "}"
    );
//    submitButton->setStyleSheet("QPushButton { color: #a9b7c6; }"); // 设置按钮的文字颜色
    connect(submitButton, &QPushButton::clicked,
            this, &MainWindow::onSubmitButtonClicked); // 连接提交按钮的点击事件到相应的处理函数


    /*****创建用于显示消息的文本框*****/
    messageDisplay = new QTextEdit(this);  // 创建用于显示消息的文本框
    messageDisplay->setReadOnly(true);  // 设置为只读，用户不能编辑其中的内容
    messageDisplay->setGeometry(370, 30, 400, 280);  // 设置位置和大小
    messageDisplay->setStyleSheet("QTextEdit { background-color: #353740; color: #6e6e80;border: 1px solid #3c3f41; }");  // 设置样式表

    /*****创建导出按钮*****/
    // 在消息显示框下面添加导出按钮
    exportButton = new QPushButton("消息导出", this);
    exportButton->move(370, 320);
    exportButton->setStyleSheet(
            "QPushButton {"
            "border: 1px solid #353740;"
            "border-radius: 10px;"
            "background-color: #569142;"
            "color: #a9b7c6;"
            "}"
            "QPushButton:pressed {"
            "background-color: #353740;"
            "}"
    );
    connect(exportButton, &QPushButton::clicked,
            this, &MainWindow::onExportButtonClicked);

    // 在所有控件初始化完成后，调用 onTopicCountChanged 函数，以生成一个默认的主题输入框
    onTopicCountChanged(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

