//
// Created by jinyi on 2023/6/6.
//
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDateTime>
#include <QLabel>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>

static const int TOPIC_LINE_EDIT_HEIGHT = 50; //设置主题输入框的高度为30像素

void MainWindow::onTopicCountChanged(int index)
{
    int topicCount = topicCountComboBox->itemData(index).toInt(); // 获取选中的主题数量
    qDeleteAll(topicLineEdits.begin(), topicLineEdits.end()); // 删除所有旧的主题输入框
    topicLineEdits.clear(); // 清空主题输入框列表

    for (int i = 0; i < topicCount; ++i) {
        auto *lineEdit = new QLineEdit(); // 创建新的主题输入框
        lineEdit->setStyleSheet("QLineEdit { background-color: #2b2b2b; color: #a9b7c6; border: 1px solid #2b2b2b; }");
        topicLayout->addWidget(lineEdit); // 将新的主题输入框添加到布局中
        topicLineEdits.append(lineEdit); // 将新的主题输入框添加到列表中
    }
    // 计算并设置QScrollArea的高度
    int scrollAreaHeight = std::min(TOPIC_LINE_EDIT_HEIGHT * topicCount, 300);
    topicScrollArea->setGeometry(50, 170, 310, scrollAreaHeight);

    // 设置Submit按钮的位置，始终位于QScrollArea下方20像素的位置
    submitButton->move(50, 170 + scrollAreaHeight + 20);
}

void MainWindow::onSubmitButtonClicked()
{
    QString broker = brokerLineEdit->text(); // 获取 MQTT Broker
    QString username = usernameLineEdit->text(); // 获取用户名
    QString password = passwordLineEdit->text(); // 获取密码

    qDebug() << "Broker: " << broker;
    qDebug() << "Username: " << username;
    qDebug() << "Password: " << password;

    for (QLineEdit *lineEdit : topicLineEdits) { // 遍历所有主题输入框
        qDebug() << "Topic: " << lineEdit->text(); // 获取并打印主题
    }

    setupMQTTClient();
}


void MainWindow::onAboutClicked()
{
    QMessageBox::about(this, "关于", "作者: 金一涛\n版本: v0.0.1");
}

/*****导出消息*****/
void MainWindow::onExportButtonClicked() {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Export Messages"), "",
                                                    tr("Text Files (*.log);;All Files (*)"));

    if (fileName.isEmpty())
        return;
    else {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"),
                                     file.errorString());
            return;
        }
        QTextStream out(&file);
        out << messageDisplay->toPlainText();
    }
}

void MainWindow::setupMQTTClient() {
    QString brokerUrl = brokerLineEdit->text(); // 获取用户输入的broker URL
    // 获取当前时间的时间戳，并将其转化为字符串格式作为客户端 ID
    QString clientId = QString::number(QDateTime::currentDateTime().toSecsSinceEpoch());
    MQTTAsync_create(&client, brokerUrl.toStdString().c_str(), clientId.toStdString().c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    conn_opts.onSuccess = MainWindow::onConnected;
    conn_opts.context = this;

    // Get username and password from user input
    QString username = usernameLineEdit->text(); // 获取用户输入的用户名
    QString password = passwordLineEdit->text(); // 获取用户输入的密码
    // Set username and password for connection
    conn_opts.username = username.toStdString().c_str();
    conn_opts.password = password.toStdString().c_str();

    MQTTAsync_setCallbacks(client, this, NULL, MainWindow::onMessageArrived, NULL);
    MQTTAsync_connect(client, &conn_opts);
}

void MainWindow::onConnected(void* context, MQTTAsync_successData* response) {
    auto* window = (MainWindow*)context;
    qDebug() << "Connected to MQTT broker.";
    for (QLineEdit* lineEdit : window->topicLineEdits) {
        QString topic = lineEdit->text();
        MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
        opts.onSuccess = MainWindow::onSubscribed;
        opts.context = window;
        MQTTAsync_subscribe(window->client, topic.toStdString().c_str(), 1, &opts);  // use window->client instead of client
    }
}

void MainWindow::onSubscribed(void* context, MQTTAsync_successData* response) {
    qDebug() << "Subscribed to topic.";
}

int MainWindow::onMessageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message) {
    auto* window = (MainWindow*)context;
    QString topic = QString::fromUtf8(topicName, topicLen);
    QString payload = QString::fromUtf8((char*)message->payload, message->payloadlen);

    // 获取当前时间并转换为字符串
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    qDebug() << "Received message: Topic:" << topic << "Payload:" << payload;

    // 在消息前面加上当前时间
    window->messageDisplay->append(currentTime + " " + topic + ": " + payload);

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}