#ifndef AICHAT_H
#define AICHAT_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include<QJsonArray>
#include <QListWidget>
#include <QSplitter>
#include <QScreen>
#include <QDateTime>
#include<QDialog>
#include<QComboBox>
#include<QLabel>
#include <QCloseEvent>

class aichat : public QWidget
{
    Q_OBJECT

public:
    explicit aichat(QWidget *parent = nullptr,QString apiKey="",QString apiEndpoint="");
    ~aichat();

private:
   // Ui::MainWindow *ui;
    QString m_apiKey;
    QString m_apiEndpoint;
    QComboBox* model;
    QTextEdit* chatTextEdit;
    QLineEdit* chatInputLine;
    QPushButton* sendButton;
    QNetworkAccessManager* networkManager;
    QPushButton *cancelButton;
    QNetworkReply *currentReply;
    QPushButton *newChatButton;
    QPushButton *historyButton;
    QDialog *historyDialog;
    QListWidget *historyList;
    
    QString currentChatId;
    QMap<QString, QJsonArray> chatHistories;
    QJsonArray chatHistory;
    QStringList modelist;

    void toggleChatWidget();
    void sendMessage();
    void sendMessageToAI(const QString &message);
    void handleAIResponse();
    void showHistory();
    void createNewChat();
    void loadChat(QListWidgetItem *item);
    void saveCurrentChat();
    void updateHistoryList();
    void loadChat(const QString &chatId);
    void initmodelist();
    void closeEvent(QCloseEvent *event) override;
private slots:
    // ... 其他槽函数 ...
    void handleNetworkError(QNetworkReply::NetworkError error);
    void cancelRequest();
    void deleteSelectedChat();
    void loadSelectedChat(QListWidgetItem *item);

};

#endif // AICHAT_H
