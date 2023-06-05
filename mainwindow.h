
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>

#include "Mqtt_lib/src/MQTTAsync.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr); // 构造函数
    ~MainWindow(); // 析构函数

private slots:
    void onSubmitButtonClicked(); // 提交按钮的点击事件处理函数
    void onTopicCountChanged(int index); // 主题数量选择框的选择改变事件处理函数
    void onAboutClicked(); //"关于"菜单项
    void setupMQTTClient();
    void onExportButtonClicked(); // 导出按钮的点击事件处理函数


private:
    Ui::MainWindow *ui;
    QLineEdit *brokerLineEdit; // MQTT Broker 输入框
    QLineEdit *usernameLineEdit; // 用户名输入框
    QLineEdit *passwordLineEdit; // 密码输入框
    QComboBox *topicCountComboBox; // 主题数量选择框
    QScrollArea *topicScrollArea; // 主题输入框的滚动区域
    QVBoxLayout *topicLayout; // 主题输入框的布局
    QList<QLineEdit*> topicLineEdits; // 主题输入框列表
    QPushButton *submitButton; // 提交按钮
    QTextEdit *messageDisplay;  // 用于显示消息的文本框
    MQTTAsync client;
    static void onConnected(void* context, MQTTAsync_successData* response);
    static void onSubscribed(void* context, MQTTAsync_successData* response);
    static int onMessageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
    QPushButton *exportButton; // 导出按钮
};

#endif // MAINWINDOW_H
