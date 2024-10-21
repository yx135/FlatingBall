#pragma once

#include <QWidget>
#include <QRect>
#include <QMouseEvent>
#include <QPaintEvent>
#include<QPainter>
class ScreenshotSelector : public QWidget
{
    Q_OBJECT

public:
    ScreenshotSelector(QWidget *parent = nullptr);
    QRect getSelectedRect() const { return selectedRect; }

signals:
    void screenshotTaken(const QRect &rect);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool selecting;
    QPoint startPos;
    QRect selectedRect;
};
