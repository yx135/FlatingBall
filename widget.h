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

    void saveSettings();
    void loadSettings();

public slots:
    void takeScreenshot();
    void handleSingleClick();
      void showContextMenu(const QPoint &pos);
    void showSettings();
    void exitApplication();
};


class ScreenshotSelector : public QDialog {
public:
    ScreenshotSelector(QWidget *parent = nullptr) : QDialog(parent) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setGeometry(QApplication::desktop()->screenGeometry());

        rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
        rubberBand->setStyleSheet("border: 2px solid red;");
    }

    ~ScreenshotSelector() {
        qDebug() << "ScreenshotSelector destructor called";
    }

    QRect getSelectedRect() const {
        return selectedRect;
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(0, 0, 0, 32));
    }

    void mousePressEvent(QMouseEvent *event) override {
        origin = event->pos();
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        selectedRect = QRect(origin, event->pos()).normalized();
        accept();
    }

private:
    QPoint origin;
    QRect selectedRect;
    QRubberBand *rubberBand;
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