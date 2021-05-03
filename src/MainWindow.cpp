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

    calculateShifts();

    auto x = calculateScaledX(0);
    auto y = calculateScaledY(0);
    m_renderData.referencePoint = {x, y};

    new QShortcut(QKeySequence(Qt::Key_P), this, [&](){
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
    static bool isFirstEnter{true};
    grabScreen();
#ifdef Q_OS_WIN
    setWindowOpacity(1);
#endif
    m_renderData.isActivated = true;

    if (!isFirstEnter)
    {
        adjust(m_lastWindowPos.x() - pos().x(),
               m_lastWindowPos.y() - pos().y());
    }

    isFirstEnter = false;

    update();

    event->accept();
}

void MainWindow::leaveEvent(QEvent* event)
{
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif
    m_renderData.isActivated = false;
    m_lastWindowPos = pos();

    update();

    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    auto x = calculateScaledX(event->x());
    auto y = calculateScaledY(event->y());

    if ( event->buttons() & Qt::LeftButton )
    {
        m_renderData.referencePoint = {x, y};
    }

    m_renderData.cursorPoint = {x, y};

    calculate();
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
    auto x = calculateScaledX(event->x());
    auto y = calculateScaledY(event->y());

    if (event->button() == Qt::RightButton)
    {
        setReferenceRectangle();
    }
    else
    {
        m_renderData.referencePoint = {x, y};
    }

    m_renderData.cursorPoint = {x, y};

    calculate();
    update();

    event->accept();
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
    if (m_renderData.fixedRectangles.contains(m_renderData.cursorRectangle))
    {
        m_renderData.fixedRectangles.removeOne(m_renderData.cursorRectangle);
    }
    else
    {
        m_renderData.fixedRectangles.append(m_renderData.cursorRectangle);
    }
}

void MainWindow::calculate()
{
    auto x = m_renderData.cursorPoint.x();
    auto y = m_renderData.cursorPoint.y();
    auto px = m_renderData.referencePoint.x();
    auto py = m_renderData.referencePoint.y();
    auto w = m_renderData.screenImage.width();
    auto h = m_renderData.screenImage.height();
    auto color = m_renderData.screenImage.pixel(x, y);
    auto r = beamTo(x, w - 1, y, 1, Qt::Horizontal, color);
    auto l = beamTo(x, 0, y, -1, Qt::Horizontal, color);
    auto b = beamTo(y, h - 1, x, 1, Qt::Vertical, color);
    auto t = beamTo(y, 0, x, -1, Qt::Vertical, color);

    m_renderData.cursorHLine = {l, y, r, y};
    m_renderData.cursorVLine = {x, t, x, b};
    m_renderData.cursorRectangle = {l, t, r - l, b - t};
    m_renderData.measureHLine = {x, y, px, y};
    m_renderData.measureVLine = {x, y, x, py};

    if (m_renderData.cursorRectangle.contains(px, py))
    {
        m_renderData.referenceHLine = {px, py, l, py};
        m_renderData.referenceVLine = {px, py, px, t};
    }
    else
    {
        if (b < py)
        {
             m_renderData.referenceVLine = {px, py, px, b};
        }
        else
        {
             m_renderData.referenceVLine = {px, py, px, t};
        }

        if (r < px)
        {
            m_renderData.referenceHLine = {px, py, r, py};
        }
        else
        {
            m_renderData.referenceHLine = {px, py, l, py};
        }
    }
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

void MainWindow::calculateShifts()
{
    m_renderData.scaleShiftX = (rect().width() - rect().width() / m_renderData.scale) / 2;
    m_renderData.scaleShiftY = (rect().height() - rect().height() / m_renderData.scale) / 2;
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

void MainWindow::switchPalette()
{
    if(++m_paletteIndex >= m_palettes.size())
    {
        m_paletteIndex = 0;
    }
    m_painter.setPalette(m_palettes[m_paletteIndex]);
}

void MainWindow::adjust(int dx, int dy)
{
    m_renderData.referencePoint.rx() += dx;
    m_renderData.referencePoint.ry() += dy;

    for (auto& rectangle : m_renderData.fixedRectangles)
    {
        rectangle.translate(dx, dy);
    }
}
