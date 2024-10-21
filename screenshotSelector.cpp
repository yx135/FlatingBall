#include "screenshotSelector.h"

ScreenshotSelector::ScreenshotSelector(QWidget *parent) : QWidget(parent), selecting(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setCursor(Qt::CrossCursor);
}

void ScreenshotSelector::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(0, 0, 0, 100));

    if (!selectedRect.isNull())
    {
        painter.fillRect(selectedRect, QColor(255, 255, 255, 100));
        painter.setPen(QPen(Qt::red, 2));
        painter.drawRect(selectedRect);
    }
}

void ScreenshotSelector::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        selecting = true;
        startPos = event->pos();
        selectedRect = QRect();
        update();
    }
}

void ScreenshotSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (selecting)
    {
        selectedRect = QRect(startPos, event->pos()).normalized();
        update();
    }
}

void ScreenshotSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        selecting = false;
        selectedRect = QRect(startPos, event->pos()).normalized();
        update();
        emit screenshotTaken(selectedRect);
        close();
    }
}