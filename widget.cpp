#include "widget.h"
#include"aichat.h"



FloatingBall::FloatingBall(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
      m_isExpanded(false),isExpanded(false)
{
    qDebug()<<m_apiKey;
    setAttribute(Qt::WA_TranslucentBackground);
    m_collapsedSize = QSize(50, 50);
    m_expandedSize = QSize(200, 200);
    setFixedSize(m_collapsedSize);

    doubleClickTimer = new QTimer(this);
    doubleClickTimer->setSingleShot(true);
    connect(doubleClickTimer, &QTimer::timeout, this, &FloatingBall::handleSingleClick);

    //设置窗口透明
    setAttribute(Qt::WA_TranslucentBackground);

      // 设置窗口整体透明度
   setWindowOpacity(0.4); // 0.0 完全透明，1.0 完全不透明


    m_animation = new QPropertyAnimation(this, "size");
    m_animation->setDuration(300);

    QVBoxLayout *layout = new QVBoxLayout(this);
   /// layout->addWidget(new QLabel("额外信息", this));
   // layout->addWidget(new QLabel("更多控件", this));

    // 添加截图按钮
    screenshotButton = new QPushButton("截图", this);

    collapsedSize = QSize(50, 50); // 根据需要调整大小
    expandedSize = QSize(150, 150); // 根据需要调整大小
    setFixedSize(collapsedSize); // 设置初始大小为折叠状态

    connect(screenshotButton, &QPushButton::clicked, this, &FloatingBall::takeScreenshot);
    layout->addWidget(screenshotButton);


    //添加聊天功能
    this->aichatButton = new QPushButton("聊天",this);
    layout->addWidget(aichatButton);
    
    connect(aichatButton,&QPushButton::clicked,this,[&](){
        qDebug()<<"构建aichat";
        qDebug()<<this->m_apiEndpoint<<"++"<<this->m_apiKey;
        aichat* chatWindow = new aichat(nullptr, m_apiKey, m_apiEndpoint);
        chatWindow->setAttribute(Qt::WA_DeleteOnClose);
        chatWindow->show();
    });
    layout->setAlignment(Qt::AlignCenter);
     // 添加右键菜单
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &FloatingBall::customContextMenuRequested, this, &FloatingBall::showContextMenu);

    // 加载设置
    loadSettings();
    qDebug()<<m_apiKey;

}

void FloatingBall::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
            if (isWaitingForDoubleClick) {
                doubleClickTimer->stop();
                isWaitingForDoubleClick = false;
                toggleExpansion();
            } else {
                isWaitingForDoubleClick = true;
                doubleClickTimer->start(QApplication::doubleClickInterval());
            }
        }
        QWidget::mousePressEvent(event);
}

void FloatingBall::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void FloatingBall::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        toggleExpansion();
        event->accept();
    }
}

void FloatingBall::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
       painter.setRenderHint(QPainter::Antialiasing);

       if (isExpanded) {
           // 绘制展开状态的外观
           painter.setBrush(QColor(200, 200, 200, 200));
           painter.drawRoundedRect(rect(), 10, 10);
           screenshotButton->show(); // 在展开状态显示按钮
           aichatButton->show();
       } else {
           // 绘制缩放状态的外观
           painter.setBrush(QColor(150, 150, 150, 200));
           painter.drawEllipse(rect());
           screenshotButton->hide(); // 在折叠状态隐藏按钮
           aichatButton->hide();
       }
}

void FloatingBall::toggleExpansion()
{
     QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    if (!isExpanded) {
        // 展开
        m_originalPos = globalPos; // 保存原始位置
        if (globalPos.x() < screenGeometry.width() / 2) {
            // 在左半边屏幕，向右展开
            setFixedSize(expandedSize);
        } else {
            // 在右半边屏幕，向左展开
            QPoint newPos = globalPos;
            newPos.setX(globalPos.x() - (expandedSize.width() - collapsedSize.width()));
            setFixedSize(expandedSize);
            move(newPos);
        }
        screenshotButton->show();
        aichatButton->show();
    } else {
        // 折叠
        setFixedSize(collapsedSize);
        move(m_originalPos); // 恢复到原始位置
        screenshotButton->hide();
        aichatButton->hide();
    }

    isExpanded = !isExpanded;
    update(); // 触发重绘
}

void FloatingBall::setSize(const QSize &size)
{
    setFixedSize(size);
}


// 在 FloatingBall 类的析构函数中添加以下代码
FloatingBall::~FloatingBall()
{
    qDebug() << "FloatingBall destructor called";
}
void FloatingBall::closeEvent(QCloseEvent *event)
{
    qDebug() << "Close event received";
    //event->ignore();  // 忽略关闭事件，防止窗口被关闭
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "退出确认", "确定要退出程序吗？",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
          event->accept();
          QApplication::quit();
      } else {
          event->ignore();
        }
 }

void FloatingBall::takeScreenshot()
{
    qDebug() << "takeScreenshot function started";

    // 隐藏浮动球以避免它出现在截图中
    this->hide();

    QTimer::singleShot(200, this, [this]() {
        qDebug() << "Timer callback started";
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            ScreenshotSelector selector;
            if (selector.exec() == QDialog::Accepted) {
                QRect selectionRect = selector.getSelectedRect();
                if (!selectionRect.isNull()) {
                    QPixmap screenshot = screen->grabWindow(0, selectionRect.x(), selectionRect.y(),
                                                            selectionRect.width(), selectionRect.height());

                    QString fileName = QFileDialog::getSaveFileName(this, "保存截图",
                                                                    QDir::homePath() + "/screenshot.png",
                                                                    "Images (*.png *.jpg)");
                    if (!fileName.isEmpty()) {
                        if (screenshot.save(fileName)) {
                            qDebug() << "Screenshot saved successfully to:" << fileName;
                        } else {
                            qDebug() << "Failed to save screenshot to:" << fileName;
                        }
                    }
                }
            }
        }

        // 显示浮动球
        this->show();
        qDebug() << "Timer callback ended";
    });

    qDebug() << "takeScreenshot function ended";
}

void FloatingBall::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
        if (isExpanded) {
            // 调整按钮位置
            screenshotButton->setGeometry(10, height() - 40, width() - 20, 30);
        }
}

void FloatingBall::handleSingleClick()
{
    isWaitingForDoubleClick = false;
    // 在这里处理单击事件，如果需要的话
}

//右键菜单功能：
void FloatingBall::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("悬浮球菜单"), this);

    QAction settingsAction(tr("设置"), this);
    connect(&settingsAction, &QAction::triggered, this, &FloatingBall::showSettings);
    contextMenu.addAction(&settingsAction);

    QAction exitAction(tr("退出"), this);
    connect(&exitAction, &QAction::triggered, this, &FloatingBall::exitApplication);
    contextMenu.addAction(&exitAction);

    contextMenu.exec(mapToGlobal(pos));
}


void FloatingBall::showSettings()
{
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle(tr("设置"));
    settingsDialog.setMinimumSize(400, 300);  // 设置最小尺寸

    QHBoxLayout *mainLayout = new QHBoxLayout(&settingsDialog);

    QListWidget *tabList = new QListWidget(&settingsDialog);
    tabList->addItem(tr("AI聊天"));
    tabList->setFixedWidth(100);  // 设置固定宽度
    mainLayout->addWidget(tabList);

    QStackedWidget *stackedWidget = new QStackedWidget(&settingsDialog);
    mainLayout->addWidget(stackedWidget);

QWidget *chatSettingsPage = new QWidget();
QFormLayout *chatLayout = new QFormLayout(chatSettingsPage);
AutoResizingPlainTextEdit *apiKeyEdit = new AutoResizingPlainTextEdit(m_apiKey, chatSettingsPage);
apiKeyEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);



chatLayout->addRow(tr("AI Chat API Key:"), apiKeyEdit);

AutoResizingPlainTextEdit *apiEndpointEdit = new AutoResizingPlainTextEdit(m_apiEndpoint, chatSettingsPage);
apiEndpointEdit->setMinimumWidth(300);
chatLayout->addRow(tr("AI Chat API 端点:"), apiEndpointEdit);


    stackedWidget->addWidget(chatSettingsPage);

    // 连接列表项选择信号到堆叠窗口
    connect(tabList, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    // 默认选中第一项
    tabList->setCurrentRow(0);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, &settingsDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &settingsDialog, &QDialog::reject);
    mainLayout->addWidget(buttonBox);

    if (settingsDialog.exec() == QDialog::Accepted) {
        m_apiKey = apiKeyEdit->toPlainText();;
        m_apiEndpoint = apiEndpointEdit->toPlainText();
        saveSettings();
    }
}


void FloatingBall::exitApplication()
{
    QApplication::quit();
}

void FloatingBall::saveSettings()
{
    QSettings settings("YourCompany", "FloatingBall");
    settings.setValue("apiKey", m_apiKey);
    settings.setValue("apiEndpoint", m_apiEndpoint);
}

void FloatingBall::loadSettings()
{
    qDebug() << "loadSettings";
  //  qDebug() << "初始值：" << m_apiKey;
   // qDebug() << "初始值：" << m_apiEndpoint;

    QSettings settings("YourCompany", "FloatingBall");
    
    // 使用空字符串作为默认值，这样可以检测设置是否存在
    QString loadedApiKey = settings.value("apiKey", "").toString();
    QString loadedApiEndpoint = settings.value("apiEndpoint", "").toString();

    // 只有在设置存在时才更新值
    if (!loadedApiKey.isEmpty()) {
        m_apiKey = loadedApiKey;
    }
    if (!loadedApiEndpoint.isEmpty()) {
        m_apiEndpoint = loadedApiEndpoint;
    }

    //qDebug() << "加载后：" << m_apiKey;
    //qDebug() << "加载后：" << m_apiEndpoint;
}