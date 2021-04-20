#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include "MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_isActivated(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlags(Qt::WindowStaysOnTopHint |
                   Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    setMouseTracking(true);
}

void MainWindow::paintEvent(QPaintEvent*)
{
    draw();
}

void MainWindow::enterEvent(QEvent*)
{
    grabScreen();
    m_isActivated = true;
    update();
}

void MainWindow::leaveEvent(QEvent*)
{
    m_isActivated = false;
    update();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    calculateMeasurer(event->x(), event->y());
    update();
}

void MainWindow::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    m_screenImage = screen->grabWindow(descktop->winId()).toImage().copy(geometry());
}

void MainWindow::calculateMeasurer(int cx, int cy)
{
    auto w = m_screenImage.width();
    auto h = m_screenImage.height();
    auto color = m_screenImage.pixel(cx, cy);
    auto rx = measTo(cx, w - 1, cy, 1, Qt::Horizontal, color);
    auto lx = measTo(cx, 0, cy, -1, Qt::Horizontal, color);
    auto by = measTo(cy, h - 1, cx, 1, Qt::Vertical, color);
    auto ty = measTo(cy, 0, cx, -1, Qt::Vertical, color);

    m_centerPoint = QPoint(cx, cy);
    m_centerHLine = QLine(lx, cy, rx, cy);
    m_centerVLine = QLine(cx, ty, cx, by);
    m_rectangle = QRect(lx, ty, m_centerHLine.dx(), m_centerVLine.dy());
}

int MainWindow::measTo(int startPos, int endPos, int coord, int step,
                       Qt::Orientation orientation, const QRgb& color)
{
    int resPos = endPos;
    for (int pos = startPos + step; pos != endPos; pos += step)
    {
        auto point = orientation == Qt::Horizontal ? QPoint(pos, coord)
                                                   : QPoint(coord, pos);

        if (!m_screenImage.rect().contains(point))
        {
            break;
        }

        if (m_screenImage.pixel(point) != color)
        {
            return pos - step;
        }
    }
    return resPos;
}

void MainWindow::drawRuller(QPainter& painter)
{
    painter.setPen(Qt::white);
    painter.drawRect(QRect{0, 0, rect().width() - 1, rect().height() - 1});
}

void MainWindow::drawMeasurer(QPainter& painter)
{
    painter.setPen(Qt::magenta);
    painter.drawRect(m_rectangle);

    painter.setPen(Qt::cyan);
    painter.drawLine(m_centerHLine);
    painter.drawLine(m_centerVLine);

    painter.drawText(m_centerPoint.x() + 6, m_centerPoint.y() - 4,
                     QString("h: %1").arg(m_centerVLine.dy() + 1));

    painter.drawText(m_centerPoint.x() + 6, m_centerPoint.y() - 16,
                     QString("w: %1").arg(m_centerHLine.dx() + 1));
}

void MainWindow::draw()
{
    if (m_isActivated)
    {
        QPainter painter(this);

        painter.drawImage(rect(), m_screenImage);

        drawRuller(painter);
        drawMeasurer(painter);

        painter.end();
    }
}
