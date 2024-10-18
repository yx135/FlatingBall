#include "aichat.h"
#include<QWidget>
#include <QApplication> 

aichat::aichat(QWidget *parent,QString apiKey,QString apiEndpoint) : QWidget(parent)
{
    qDebug()<<"开始aichat";
        // 设置窗口大小为屏幕的四分之一宽度，一半高度
    qDebug()<<"构建参数"<<apiKey<<apiEndpoint;
    this->m_apiKey=apiKey;
    this->m_apiEndpoint=apiEndpoint;
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() /6;
    int height = screenGeometry.height() / 2;
    setGeometry(screenGeometry.width() - width, 0, width, height);

    // 创建聊天界面
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(5); // 设置布局中控件之间的间距

    QWidget* modelwidget = new QWidget(this);
    modelwidget->setFixedHeight(30);

    QHBoxLayout *modellayout = new QHBoxLayout(modelwidget);
    modellayout->setContentsMargins(0, 0, 0, 0); // 移除modelwidget内部的边距

    QLabel* info = new QLabel("选择模型", this);
    model = new QComboBox(this);
    this->initmodelist();
    modellayout->addWidget(info);
    modellayout->addWidget(model);




    chatTextEdit = new QTextEdit(this);
    chatTextEdit->setReadOnly(true);
    chatInputLine = new QLineEdit(this);
    sendButton = new QPushButton("发送", this);
    cancelButton = new QPushButton("取消", this);
    cancelButton->hide();
    newChatButton = new QPushButton("新建对话", this);
    historyButton = new QPushButton("历史记录", this);
    mainLayout->addWidget(modelwidget);
    mainLayout->addWidget(chatTextEdit);
    mainLayout->addWidget(chatInputLine);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(sendButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(newChatButton);
    buttonLayout->addWidget(historyButton);
    
    mainLayout->addLayout(buttonLayout);
    
    
     // 创建历史记录弹出窗口
    historyDialog = new QDialog(this);
    historyDialog->setWindowTitle("历史记录");
    QVBoxLayout *historyLayout = new QVBoxLayout(historyDialog);
    historyList = new QListWidget(historyDialog);
    QPushButton *deleteHistoryButton = new QPushButton("删除选中的对话", historyDialog);
    historyLayout->addWidget(historyList);
    historyLayout->addWidget(deleteHistoryButton);
    historyDialog->setLayout(historyLayout);

    connect(deleteHistoryButton, &QPushButton::clicked, this, &aichat::deleteSelectedChat);

    
    // 连接信号和槽
    connect(sendButton, &QPushButton::clicked, this, &aichat::sendMessage);
    connect(chatInputLine, &QLineEdit::returnPressed, this, &aichat::sendMessage);
    connect(cancelButton, &QPushButton::clicked, this, &aichat::cancelRequest);
    connect(newChatButton, &QPushButton::clicked, this, &aichat::createNewChat);
    connect(historyButton, &QPushButton::clicked, this, &aichat::showHistory);
    connect(historyList, &QListWidget::itemDoubleClicked, this, &aichat::loadSelectedChat);
    connect(deleteHistoryButton, &QPushButton::clicked, this, &aichat::deleteSelectedChat);
    
    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);
    
    // 创建新对话

   // 设置窗口标志
    // 修改窗口标志
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowWithoutActivating);
    createNewChat();

}
void aichat::saveCurrentChat()
{
    if (!currentChatId.isEmpty()) {
        chatHistories[currentChatId] = chatHistory;
    }
}

void aichat::deleteSelectedChat()
{
    QListWidgetItem *selectedItem = historyList->currentItem();
    if (selectedItem) {
        QString chatId = selectedItem->data(Qt::UserRole).toString();
        
        // 如果删除的是当前对话，清空聊天窗口
        if (chatId == currentChatId) {
            chatTextEdit->clear();
            currentChatId = "";
            chatHistory = QJsonArray();
        }
        
        // 从历史记录中删除对话
        chatHistories.remove(chatId);
        delete historyList->takeItem(historyList->row(selectedItem));
        
        // 如果删除后没有对话了，创建一个新的
        if (chatHistories.isEmpty()) {
            createNewChat();
        }
    }
}

void aichat::loadSelectedChat(QListWidgetItem *item)
{
    if (item) {
        QString chatId = item->data(Qt::UserRole).toString();
        loadChat(chatId);
        historyDialog->hide();
    }
}

aichat::~aichat()
{
    //delete ui;
}

void aichat::toggleChatWidget()
{
    if (isVisible()) {
        hide();
    } else {
        show();
    }
}

void aichat::sendMessage()
{
    QString message = chatInputLine->text();
    if (message.isEmpty()) return;

    chatTextEdit->append("你: " + message);
    chatInputLine->clear();
    
    // 如果是新对话的第一条消息，更新历史记录名称
    if (chatHistory.size() == 1) { // 只有欢迎消息时
        QString chatName = message.left(10); // 取前10个字符
        chatHistories.remove(currentChatId);
        currentChatId = chatName;
        chatHistories[currentChatId] = chatHistory;
        updateHistoryList();
    }
    
    chatHistory.append(QJsonObject{{"role", "user"}, {"content", message}});
    chatHistories[currentChatId] = chatHistory;

    // 更新UI状态
    chatInputLine->setEnabled(false);
    sendButton->setText("发送中...");
    sendButton->setEnabled(false);
    cancelButton->show();

    sendMessageToAI(message);
}

void aichat::createNewChat()
{
    saveCurrentChat();
    currentChatId = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    chatHistories[currentChatId] = QJsonArray();
    chatHistory = QJsonArray();
    updateHistoryList();
    chatTextEdit->clear();
    chatInputLine->clear();

    // 添加欢迎消息
    QString welcomeMessage = "欢迎开始新的对话！";
    chatTextEdit->append("系统: " + welcomeMessage);
    chatHistory.append(QJsonObject{{"role", "system"}, {"content", welcomeMessage}});
}
void aichat::loadChat(const QString &chatId)
{
    saveCurrentChat();
    currentChatId = chatId;
    chatTextEdit->clear();
    chatHistory = chatHistories[currentChatId];
    for (const QJsonValue &message : chatHistory) {
        QJsonObject messageObj = message.toObject();
        QString role = messageObj["role"].toString();
        QString content = messageObj["content"].toString();
        chatTextEdit->append(role + ": " + content);
    }
}


void aichat::sendMessageToAI(const QString &message)
{
    qDebug() << "正在发送消息到 AI:" << message;

    QUrl url(m_apiEndpoint);
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(m_apiKey).toUtf8());
    qDebug()<<m_apiEndpoint;
    qDebug()<<m_apiKey;
    qDebug()<<this->model->currentText();
    QJsonObject json;
    json["model"] = this->model->currentText();
    QJsonArray messages;
    messages.append(QJsonObject{{"role", "user"}, {"content", message}});
    json["messages"] = messages;

     currentReply = networkManager->post(request, QJsonDocument(json).toJson());
        qDebug()<<"已经发送";
    connect(currentReply, &QNetworkReply::finished, this, &aichat::handleAIResponse);
    connect(currentReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleNetworkError(QNetworkReply::NetworkError)));

}

void aichat::handleAIResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        qDebug() << "收到 AI 响应:" << response;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonResponse.object();
        QString aiMessage = jsonObject["choices"].toArray().first().toObject()["message"].toObject()["content"].toString();
        chatTextEdit->append("AI: " + aiMessage);
        // 将 AI 响应添加到对话历史
        chatHistory = chatHistories[currentChatId];
        chatHistory.append(QJsonObject{{"role", "assistant"}, {"content", aiMessage}});
    chatHistories[currentChatId] = chatHistory;


    } else {
        qDebug() << "AI 响应错误:" << reply->errorString();
        //chatTextEdit->append("AI: 抱歉，发生了错误，无法获取回复。");
    }
    reply->deleteLater();
    // 恢复UI状态
    chatInputLine->setEnabled(true);
    sendButton->setText("发送");
    sendButton->setEnabled(true);
    cancelButton->hide();
    
    currentReply = nullptr;
}

void aichat::handleNetworkError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "网络错误:" << error << "-" << reply->errorString();
    chatTextEdit->append("AI: 网络错误，无法连接到服务器。");
    reply->deleteLater();
        // 恢复UI状态
    chatInputLine->setEnabled(true);
    sendButton->setText("发送");
    sendButton->setEnabled(true);
    cancelButton->hide();
    
    currentReply = nullptr;
}
void aichat::cancelRequest()
{
    if (currentReply) {
        currentReply->abort();
        currentReply->deleteLater();
        currentReply = nullptr;
        
        chatTextEdit->append("系统: 请求已取消");
        
        // 恢复UI状态
        chatInputLine->setEnabled(true);
        sendButton->setText("发送");
        sendButton->setEnabled(true);
        cancelButton->hide();
    }
}
void aichat::updateHistoryList()
{
    historyList->clear();
    for (auto it = chatHistories.begin(); it != chatHistories.end(); ++it) {
        QListWidgetItem *item = new QListWidgetItem(it.key());
        item->setData(Qt::UserRole, it.key()); // 存储完整的chatId
        historyList->addItem(item);
    }
}

void aichat::showHistory()
{
    updateHistoryList();
    historyDialog->show();
}

void aichat::initmodelist()
{
    this->modelist.append("gpt-4o");
    this->modelist.append("claude-sonnet-3.5");
    this->model->addItems(this->modelist);
}
void aichat::closeEvent(QCloseEvent *event)
{
    event->accept();  // 确保窗口正常关闭
    // 不要调用 QApplication::quit()
}
