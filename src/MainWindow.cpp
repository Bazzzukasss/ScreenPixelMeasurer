#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QKeyEvent>
#include <QShortcut>
#include <QtMath>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlags(Qt::WindowStaysOnTopHint |
                   Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif

    initialize();
}

void MainWindow::initialize()
{
    m_painter.setPalette(m_palettes[m_paletteIndex]);
    m_renderData.scale = kMinScale;
    m_renderData.isActivated = false;
    m_renderData.isReferencePointPresent = false;

    calculateShifts();

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this, [&](){
        switchPalette();
        update();
    });
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    m_painter.begin(this);
    m_painter.draw(m_renderData);
    m_painter.end();

    event->accept();
}

void MainWindow::enterEvent(QEvent* event)
{
    grabScreen();
#ifdef Q_OS_WIN
    setWindowOpacity(1);
#endif
    m_renderData.isActivated = true;
    if (m_renderData.windowPos != pos())
    {
        //clearReferenceRectangle();
    }

    update();

    event->accept();
}

void MainWindow::leaveEvent(QEvent* event)
{
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif
    m_renderData.isActivated = false;
    m_renderData.windowPos = pos();
    update();

    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if ( event->buttons() & Qt::LeftButton )
    {
        setReferencePoint(event->x(), event->y());
    }
    calculateCursorRectangle(event->x(), event->y());
    calculateMeasurer();
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

void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        setReferenceRectangle();
    }
    else
    {
        setReferencePoint(event->x(), event->y());
    }

    calculateMeasurer();
    update();
}

void MainWindow::resizeEvent(QResizeEvent*)
{
    m_renderData.windowRectangle = rect();
}

void MainWindow::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    m_renderData.screenImage = screen->grabWindow(descktop->winId()).toImage().copy(geometry());
}

void MainWindow::setReferenceRectangle()
{
    if (m_renderData.referenceRectangles.contains(m_renderData.cursorRectangle))
    {
        m_renderData.referenceRectangles.removeOne(m_renderData.cursorRectangle);
    }
    else
    {
        m_renderData.referenceRectangles.append(m_renderData.cursorRectangle);
    }
}

void MainWindow::setReferencePoint(int x, int y)
{
    auto cx = calculateScaledX(x);
    auto cy = calculateScaledY(y);

    m_renderData.isReferencePointPresent = true;
    m_renderData.referencePoint = {cx, cy};
}

void MainWindow::calculateCursorRectangle(int x, int y)
{
    auto cx = calculateScaledX(x);
    auto cy = calculateScaledY(y);
    auto w = m_renderData.screenImage.width();
    auto h = m_renderData.screenImage.height();
    auto color = m_renderData.screenImage.pixel(cx, cy);
    auto rx = beamTo(cx, w - 1, cy, 1, Qt::Horizontal, color);
    auto lx = beamTo(cx, 0, cy, -1, Qt::Horizontal, color);
    auto by = beamTo(cy, h - 1, cx, 1, Qt::Vertical, color);
    auto ty = beamTo(cy, 0, cx, -1, Qt::Vertical, color);

    m_renderData.cursorPoint = QPoint(cx, cy);
    m_renderData.cursorHLine = QLine(lx, cy, rx, cy);
    m_renderData.cursorVLine = QLine(cx, ty, cx, by);
    m_renderData.cursorRectangle = QRect(lx, ty, m_renderData.cursorHLine.dx(), m_renderData.cursorVLine.dy());
}

void MainWindow::calculateMeasurer()
{
    m_renderData.measureVLine = {0, 0, 0, 0};
    m_renderData.measureHLine = {0, 0, 0, 0};

    if (m_renderData.isReferencePointPresent)
    {
        auto cx = m_renderData.cursorPoint.x();
        auto cy = m_renderData.cursorPoint.y();
        auto rx = m_renderData.referencePoint.x();
        auto ry = m_renderData.referencePoint.y();

        if (cx > rx)
        {
            m_renderData.measureHLine = {rx + 1, cy, cx, cy};
        }
        else if (cx < rx)
        {
            m_renderData.measureHLine = {rx - 1, cy, cx, cy};
        }

        if (cy > ry)
        {
            m_renderData.measureVLine = {cx, cy, cx, ry + 1};
        }
        else if (cy < ry)
        {
            m_renderData.measureVLine = {cx, cy, cx, ry - 1};
        }
    }
/*
    if (m_renderData.isReferenceRectanglePresent &&
        (m_renderData.cursorRectangle != m_renderData.referenceRectangle))
    {
        auto fcx = m_renderData.referenceRectangle.center().x();
        auto fcy = m_renderData.referenceRectangle.center().y();
        auto fr = m_renderData.referenceRectangle.right();
        auto fl = m_renderData.referenceRectangle.left();
        auto ft = m_renderData.referenceRectangle.top();
        auto fb = m_renderData.referenceRectangle.bottom();

        auto ccx = m_renderData.cursorRectangle.center().x();
        auto ccy = m_renderData.cursorRectangle.center().y();
        auto cr = m_renderData.cursorRectangle.right();
        auto cl = m_renderData.cursorRectangle.left();
        auto ct = m_renderData.cursorRectangle.top();
        auto cb = m_renderData.cursorRectangle.bottom();

        if (m_renderData.cursorRectangle.contains(m_renderData.referenceRectangle))
        {
            m_renderData.measureHLine = {fl - 1, fcy, cl, fcy};
            m_renderData.measureVLine = {fcx, ft - 1, fcx, ct};
            m_renderData.measureHValue = abs(fl - cl);
            m_renderData.measureVValue = abs(ft - ct);
        }
        else if (m_renderData.referenceRectangle.contains(m_renderData.cursorRectangle))
        {
            m_renderData.measureHLine = {cl - 1, ccy, fl, ccy};
            m_renderData.measureVLine = {ccx, ct - 1, ccx, ft};
            m_renderData.measureHValue = abs(fl - cl);
            m_renderData.measureVValue = abs(ft - ct);
        }
        else
        {
            if (m_renderData.cursorRectangle.bottom() < m_renderData.referenceRectangle.top())
            {
                m_renderData.measureVLine = {ccx, cb + 2, ccx, ft - 1};
                m_renderData.measureVValue = abs(ft - cb);
            }
            else if (m_renderData.cursorRectangle.top() > m_renderData.referenceRectangle.bottom())
            {
                m_renderData.measureVLine = {ccx, ct - 1, ccx, fb + 2};
                m_renderData.measureVValue = abs(fb - ct);
            }
            else
            {
                if (m_renderData.cursorRectangle.top() < m_renderData.referenceRectangle.top())
                {
                    m_renderData.measureVLine = {ccx, ct, ccx, ft - 1};
                }
                else if (m_renderData.cursorRectangle.top() > m_renderData.referenceRectangle.top())
                {
                    m_renderData.measureVLine = {ccx, ct - 1, ccx, ft};

                }
                m_renderData.measureVValue = abs(ft - ct);
            }

            if (m_renderData.cursorRectangle.right() < m_renderData.referenceRectangle.left())
            {
                m_renderData.measureHLine = {cr + 2, ccy, fl - 1, ccy};
                m_renderData.measureHValue = abs(cr - fl);
            }
            else if (m_renderData.cursorRectangle.left() > m_renderData.referenceRectangle.right())
            {
                m_renderData.measureHLine = {cl - 1, ccy, fr + 2, ccy};
                m_renderData.measureHValue = abs(cl - fr);
            }
            else
            {
                if (m_renderData.cursorRectangle.left() < m_renderData.referenceRectangle.left())
                {
                    m_renderData.measureHLine = {cl, ccy, fl - 1, ccy};
                }
                else if (m_renderData.cursorRectangle.left() > m_renderData.referenceRectangle.left())
                {
                    m_renderData.measureHLine = {cl - 1, ccy, fl, ccy};
                }
                m_renderData.measureHValue = abs(cl - fl);
            }
            m_renderData.measureHValue -= 2;
            m_renderData.measureVValue -= 2;
        }
    }
    */
}

int MainWindow::calculateScaledX(int x)
{
    return m_renderData.scaleShiftX + x / m_renderData.scale;
}

int MainWindow::calculateScaledY(int y)
{
    return m_renderData.scaleShiftY + y / m_renderData.scale;
}

int MainWindow::beamTo(int startPos, int endPos, int coord, int step,
                       Qt::Orientation orientation, const QRgb& color)
{
    int resPos = endPos;
    for (int pos = startPos + step; pos != endPos; pos += step)
    {
        auto point = orientation == Qt::Horizontal ? QPoint(pos, coord)
                                                   : QPoint(coord, pos);

        if (!m_renderData.screenImage.rect().contains(point))
        {
            break;
        }

        if (m_renderData.screenImage.pixel(point) != color)
        {
            return pos - step;
        }
    }
    return resPos;
}


void MainWindow::changeScale(const QPoint& delta)
{
    m_renderData.scale += delta.y() > 0 ? 1 : -1;

    if (m_renderData.scale > kMaxScale)
    {
        m_renderData.scale = kMaxScale;
    }

    if (m_renderData.scale < kMinScale)
    {
        m_renderData.scale = kMinScale;
    }

    calculateShifts();
}

void MainWindow::calculateShifts()
{
    m_renderData.scaleShiftX = (rect().width() - rect().width() / m_renderData.scale) / 2;
    m_renderData.scaleShiftY = (rect().height() - rect().height() / m_renderData.scale) / 2;
}

void MainWindow::switchPalette()
{
    if(++m_paletteIndex >= m_palettes.size())
    {
        m_paletteIndex = 0;
    }
    m_painter.setPalette(m_palettes[m_paletteIndex]);
}
