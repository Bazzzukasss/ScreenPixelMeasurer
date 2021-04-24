#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include "MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_scale(kMinScale),
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
    calculateShifts();
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    draw();

    event->accept();
}

void MainWindow::enterEvent(QEvent* event)
{
    grabScreen();
#ifdef Q_OS_WIN
    setWindowOpacity(1);
#endif
    m_isActivated = true;
    update();

    event->accept();
}

void MainWindow::leaveEvent(QEvent* event)
{
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif
    m_isActivated = false;
    update();

    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    calculateMeasurer(event->x(), event->y());
    update();

    event->accept();
}

void MainWindow::wheelEvent(QWheelEvent* event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
       changeScale(numPixels);
    }
    else if (!numDegrees.isNull())
    {
        changeScale(numDegrees);
    }

    update();

    event->accept();
}

void MainWindow::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    m_screenImage = screen->grabWindow(descktop->winId()).toImage().copy(geometry());
}

void MainWindow::calculateMeasurer(int x, int y)
{
    auto cx = m_scaleShiftX + x / m_scale;
    auto cy = m_scaleShiftY + y / m_scale;
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

void MainWindow::drawBackground(QPainter& painter)
{
    painter.drawImage(rect(), m_screenImage);
    painter.setPen(Qt::white);
    painter.drawRect(QRect{0, 0, rect().width() - 1, rect().height() - 1});
}

void MainWindow::drawMeasurer(QPainter& painter)
{
    painter.setPen(Qt::magenta);
    painter.drawRect(m_rectangle);

    painter.setPen(Qt::darkCyan);
    painter.drawLine(m_centerHLine);
    painter.drawLine(m_centerVLine);

    painter.setPen(Qt::cyan);
    QFontMetrics fm(font());
    auto textH = fm.height();
    auto textShift{2};

    auto vertValue = QString::number(m_centerVLine.dy() + 1);
    auto vertValueW = fm.horizontalAdvance(vertValue);
    auto vertValueX = m_rectangle.right() + textShift;
    auto vertValueY = m_rectangle.center().y() + textH / 4;
    if (vertValueX + vertValueW > rect().right())
    {
        vertValueX = qMin(m_rectangle.right(), rect().right()) - vertValueW - textShift;
    }

    auto horValue = QString::number(m_centerHLine.dx() + 1);
    auto horValueW = fm.horizontalAdvance(horValue);
    auto horValueX = m_rectangle.center().x() - horValueW / 2;
    auto horValueY = m_rectangle.top() - textShift;
    if (horValueY < textH + textShift)
    {
        horValueY = m_rectangle.top() + textH;
    }
    if (horValueX + horValueW / 2 > rect().right())
    {
        horValueX = rect().right() - horValueW - textShift;
    }

    painter.drawText(horValueX, horValueY, horValue);
    painter.drawText(vertValueX, vertValueY, vertValue);
}

void MainWindow::draw()
{
    if (m_isActivated)
    {
        QPainter painter(this);

        painter.scale(m_scale, m_scale);
        painter.translate(-m_scaleShiftX, -m_scaleShiftY);

        drawBackground(painter);
        drawMeasurer(painter);

        painter.end();
    }
}

void MainWindow::changeScale(const QPoint& delta)
{
    m_scale += delta.y() > 0 ? 1 : -1;

    if (m_scale > kMaxScale)
    {
        m_scale = kMaxScale;
    }

    if (m_scale < kMinScale)
    {
        m_scale = kMinScale;
    }

    calculateShifts();
}

void MainWindow::calculateShifts()
{
    m_scaleShiftX = (rect().width() - rect().width() / m_scale) / 2;
    m_scaleShiftY = (rect().height() - rect().height() / m_scale) / 2;
}
