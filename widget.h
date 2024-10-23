#ifndef FLOATINGBALL_H
#define FLOATINGBALL_H



#include <QWidget>
#include <QPropertyAnimation>
#include<QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QLabel>
#include<QPushButton>
#include<QTimer>
#include<QGuiApplication>
#include<QFileDialog>
#include<QPixmap>
#include<QScreen>
#include<QRubberBand>
#include<QRect>
#include<QPoint>
#include <QGraphicsOpacityEffect>
#include <QDialog>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include<QMessageBox>
#include <QMenu>
#include <QInputDialog>
#include <QSettings>
#include <QDialogButtonBox>
#include <QStackedWidget>
#include<QFormLayout>
#include<QPlainTextEdit>
#include<QSystemTrayIcon>
#include <QShortcut>
#include <QHotkey>
#include<QClipboard>

class FloatingBall : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QSize size READ size WRITE setSize)

public:
    FloatingBall(QWidget *parent = nullptr);
    ~FloatingBall();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void toggleExpansion();
    void setSize(const QSize &size);
    void resizeEvent(QResizeEvent *event) override;

private:
    
    bool isExpanded;
    QPoint m_dragPosition;
    bool m_isExpanded;
    QPropertyAnimation *m_animation;
    QSize m_collapsedSize;
    QSize m_expandedSize;
    QPushButton *screenshotButton;
    QPushButton *aichatButton;
    QSize expandedSize;
    QSize collapsedSize;
    QTimer *doubleClickTimer;
    bool isWaitingForDoubleClick;
    QPoint m_originalPos;//悬浮球上一次的位置
    QString m_apiKey="sk-adfrHgGRxcyfISG90135D10cEc824f14B1A15cA1Cd357a36-73";
    QString m_apiEndpoint="https://oneapidev.onrender.com/v1/chat/completions";

    void captureAndSaveScreenshot(QScreen *screen);

    void saveSettings();
    void loadSettings();

    //添加系统托盘
    void createTrayIcon();
    void toggleVisibility();
    QSystemTrayIcon*  trayIcon ;

    void setupGlobalShortcuts();
public slots:
      void takeScreenshot();
      void handleSingleClick();
       void showContextMenu(const QPoint &pos);
      void showSettings();
      void exitApplication();

};




class AutoResizingPlainTextEdit : public QPlainTextEdit
    {
    public:
        AutoResizingPlainTextEdit(const QString &text, QWidget *parent = nullptr) : QPlainTextEdit(text, parent)
        {
            setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            connect(this, &QPlainTextEdit::textChanged, this, &AutoResizingPlainTextEdit::updateGeometry);
        }

        QSize sizeHint() const override
        {
            QSize size = QPlainTextEdit::sizeHint();
            QTextDocument *doc = document();
            size.setHeight(doc->size().height() + contentsMargins().top() + contentsMargins().bottom());
            return size;
        }
    };

#endif // FLOATINGBALL_H
