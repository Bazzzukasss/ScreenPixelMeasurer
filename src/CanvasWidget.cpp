#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include "CanvasWidget.h"
#include <QDebug>

CanvasWidget::CanvasWidget(QWidget* parent):
    QWidget(parent),
    m_isActivated(false)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::WindowTransparentForInput);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    setFocusPolicy(Qt::StrongFocus);
    startTimer(100);
}

void CanvasWidget::paintEvent(QPaintEvent*)
{
    drawMeasurer();
}

void CanvasWidget::keyPressEvent(QKeyEvent* event)
{
    if ((event->key() == Qt::Key_M) && !event->isAutoRepeat())
    {
        grabScreen();
        m_isActivated = true;
    }
}

void CanvasWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (!event->isAutoRepeat())
    {
        m_isActivated = false;
        update();
    }
}

void CanvasWidget::timerEvent(QTimerEvent*)
{
    if (m_isActivated)
    {
        calculateMeasurer();
        update();
    }
}

void CanvasWidget::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    m_screenImage = screen->grabWindow(descktop->winId()).toImage();
}

void CanvasWidget::calculateMeasurer()
{
    auto cursotPos = QCursor::pos();
    auto cx = cursotPos.x();
    auto cy = cursotPos.y();
    auto w = m_screenImage.width();
    auto h = m_screenImage.height();
    auto color = m_screenImage.pixel(cx, cy);

    auto measTo = [&](int startPos, int endPos, int coord, int direction, Qt::Orientation orientation){
        int resPos = endPos;
        for (int pos = startPos; pos != endPos; pos += direction)
        {
            auto point = orientation == Qt::Horizontal ? QPoint(pos, coord)
                                                       : QPoint(coord, pos);

            if (!m_screenImage.rect().contains(point))
            {
                break;
            }

            if (m_screenImage.pixel(point) != color)
            {
                return pos;
            }
        }
        return resPos;
    };

    auto rPos = measTo(cx, w, cy, 1, Qt::Horizontal);
    auto lPos = measTo(cx, 0, cy, -1, Qt::Horizontal);
    auto tPos = measTo(cy, h, cx, 1, Qt::Vertical);
    auto bPos = measTo(cy, 0, cx, -1, Qt::Vertical);

    m_hLine = QLine(lPos, cy, rPos, cy);
    m_vLine = QLine(cx, tPos, cx, bPos);
    m_pen.setColor(Qt::magenta);
}

void CanvasWidget::drawMeasurer()
{
    if (!m_screenImage.isNull() && m_isActivated)
    {
        QPainter painter(this);
        painter.setPen(m_pen);

        painter.drawImage(
                    QRect{0, 0, 300, 300},
                    m_screenImage,
                    QRect{0, 0, m_screenImage.width(), m_screenImage.width()});

        painter.drawLine(m_hLine);
        painter.drawLine(m_vLine);

        painter.end();
    }
}
