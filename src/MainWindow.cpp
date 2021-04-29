#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QShortcut>
#include <QtMath>
#include "MainWindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    m_scale(kMinScale),
    m_isActivated(false),
    m_isFixedRectanglePresent(false),
    m_palette(kDarkPalette)
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
    m_pen.setJoinStyle(Qt::MiterJoin);
    m_pen.setWidth(1);
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif

    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_P), this, [&](){
        m_palette = (m_palette.border == kDarkPalette.border) ? kLightPalette
                                                              : kDarkPalette;
        update();
    });
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
    m_isFixedRectanglePresent = false;
    m_isActivated = false;
    update();

    event->accept();
}

void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    calculateCursorRectangle(event->x(), event->y());
    calculateMeasureRectangle();
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
    if (event->button() == Qt::LeftButton)
    {
        if (m_fixedRectangle == m_cursorRectangle)
        {
            m_isFixedRectanglePresent = false;
        }
        else
        {
            m_fixedRectangle = m_cursorRectangle;
            m_fixedHValue = m_cursorHValue;
            m_fixedVValue = m_cursorVValue;
            m_isFixedRectanglePresent = true;
        }

        update();
    }
}

void MainWindow::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    m_screenImage = screen->grabWindow(descktop->winId()).toImage().copy(geometry());
}

void MainWindow::calculateCursorRectangle(int x, int y)
{
    auto cx = m_scaleShiftX + x / m_scale;
    auto cy = m_scaleShiftY + y / m_scale;
    auto w = m_screenImage.width();
    auto h = m_screenImage.height();
    auto color = m_screenImage.pixel(cx, cy);
    auto rx = beamTo(cx, w - 1, cy, 1, Qt::Horizontal, color);
    auto lx = beamTo(cx, 0, cy, -1, Qt::Horizontal, color);
    auto by = beamTo(cy, h - 1, cx, 1, Qt::Vertical, color);
    auto ty = beamTo(cy, 0, cx, -1, Qt::Vertical, color);

    m_cursorPoint = QPoint(cx, cy);
    m_cursorHLine = QLine(lx, cy, rx, cy);
    m_cursorVLine = QLine(cx, ty, cx, by);
    m_cursorRectangle = QRect(lx, ty, m_cursorHLine.dx(), m_cursorVLine.dy());
    m_cursorHValue = m_cursorRectangle.width() + 1;
    m_cursorVValue = m_cursorRectangle.height() + 1;
}

void MainWindow::calculateMeasureRectangle()
{
    m_measureVLine = {0, 0, 0, 0};
    m_measureHLine = {0, 0, 0, 0};
    m_measureHValue = 0;
    m_measureVValue = 0;

    if (m_isFixedRectanglePresent && (m_cursorRectangle != m_fixedRectangle))
    {
        auto fcx = m_fixedRectangle.center().x();
        auto fcy = m_fixedRectangle.center().y();
        auto fr = m_fixedRectangle.right();
        auto fl = m_fixedRectangle.left();
        auto ft = m_fixedRectangle.top();
        auto fb = m_fixedRectangle.bottom();

        auto ccx = m_cursorRectangle.center().x();
        auto ccy = m_cursorRectangle.center().y();
        auto cr = m_cursorRectangle.right();
        auto cl = m_cursorRectangle.left();
        auto ct = m_cursorRectangle.top();
        auto cb = m_cursorRectangle.bottom();

        if (m_cursorRectangle.contains(m_fixedRectangle))
        {
            m_measureHLine = {fl - 1, fcy, cl, fcy};
            m_measureVLine = {fcx, ft - 1, fcx, ct};
            m_measureHValue = abs(fl - cl);
            m_measureVValue = abs(ft - ct);
        }
        else if (m_fixedRectangle.contains(m_cursorRectangle))
        {
            m_measureHLine = {cl - 1, ccy, fl, ccy};
            m_measureVLine = {ccx, ct - 1, ccx, ft};
            m_measureHValue = abs(fl - cl);
            m_measureVValue = abs(ft - ct);
        }
        else
        {
            if (m_cursorRectangle.bottom() < m_fixedRectangle.top())
            {
                m_measureVLine = {ccx, cb + 2, ccx, ft - 1};
                m_measureVValue = abs(ft - cb);
            }
            else if (m_cursorRectangle.top() > m_fixedRectangle.bottom())
            {
                m_measureVLine = {ccx, ct - 1, ccx, fb + 2};
                m_measureVValue = abs(fb - ct);
            }
            else
            {
                if (m_cursorRectangle.top() < m_fixedRectangle.top())
                {
                    m_measureVLine = {ccx, ct, ccx, ft - 1};
                }
                else if (m_cursorRectangle.top() > m_fixedRectangle.top())
                {
                    m_measureVLine = {ccx, ct - 1, ccx, ft};

                }
                m_measureVValue = abs(ft - ct);
            }

            if (m_cursorRectangle.right() < m_fixedRectangle.left())
            {
                m_measureHLine = {cr + 2, ccy, fl - 1, ccy};
                m_measureHValue = abs(cr - fl);
            }
            else if (m_cursorRectangle.left() > m_fixedRectangle.right())
            {
                m_measureHLine = {cl - 1, ccy, fr + 2, ccy};
                m_measureHValue = abs(cl - fr);
            }
            else
            {
                if (m_cursorRectangle.left() < m_fixedRectangle.left())
                {
                    m_measureHLine = {cl, ccy, fl - 1, ccy};
                }
                else if (m_cursorRectangle.left() > m_fixedRectangle.left())
                {
                    m_measureHLine = {cl - 1, ccy, fl, ccy};
                }
                m_measureHValue = abs(cl - fl);
            }
            m_measureHValue -= 2;
            m_measureVValue -= 2;
        }
    }
}

int MainWindow::beamTo(int startPos, int endPos, int coord, int step,
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
    m_pen.setColor(m_palette.border);
    painter.setPen(m_pen);
    painter.drawImage(rect(), m_screenImage);
    painter.drawRect(toFloat(QRect{0, 0, rect().width() - 1, rect().height() - 1}));
}

void MainWindow::drawMeasurer(QPainter& painter)
{
    if (m_isFixedRectanglePresent && (m_cursorRectangle != m_fixedRectangle))
    {
        m_pen.setColor(m_palette.measurerLines);
        painter.setPen(m_pen);
        if (m_measureHValue)
        {
            drawMeasurerLine(painter, m_measureHLine);
        }
        if (m_measureVValue)
        {
            drawMeasurerLine(painter, m_measureVLine);
        }
    }
}

void MainWindow::drawCursor(QPainter& painter)
{
    m_pen.setColor(m_palette.cursorLines);
    painter.setPen(m_pen);
    painter.drawLine(toFloat(m_cursorHLine));
    painter.drawLine(toFloat(m_cursorVLine));
}

void MainWindow::drawRectangles(QPainter& painter)
{
    m_pen.setColor(m_palette.cursorRectangle);
    painter.setPen(m_pen);
    painter.drawRect(toFloat(m_cursorRectangle));
    if (m_isFixedRectanglePresent && (m_cursorRectangle != m_fixedRectangle))
    {
        m_pen.setColor(m_palette.fixedRectangle);
        painter.setPen(m_pen);
        painter.drawRect(toFloat(m_fixedRectangle));
    }
}

void MainWindow::drawMeasurerLine(QPainter& painter, const QLine &line)
{
    auto vTick = line.dx() ? 2 : 0;
    auto hTick = line.dy() ? 2 : 0;
    painter.drawLine(toFloat(line));
    painter.drawLine(toFloat(QLine{line.x1() - hTick, line.y1() - vTick,
                                   line.x1() + hTick, line.y1() + vTick}));
    painter.drawLine(toFloat(QLine{line.x2() - hTick, line.y2() - vTick,
                                   line.x2() + hTick, line.y2() + vTick}));
}

void MainWindow::drawValues(QPainter& painter)
{
    drawValue(painter, m_measureHLine, m_measureHValue, m_palette.measurerLines);
    drawValue(painter, m_measureVLine, m_measureVValue, m_palette.measurerLines);

    drawValue(painter, {m_cursorRectangle.bottomRight(), m_cursorRectangle.topRight()}, m_cursorVValue, m_palette.cursorRectangle);
    drawValue(painter, {m_cursorRectangle.topLeft(), m_cursorRectangle.topRight()}, m_cursorHValue, m_palette.cursorRectangle);

    if (m_isFixedRectanglePresent)
    {
        drawValue(painter, {m_fixedRectangle.bottomRight(), m_fixedRectangle.topRight()}, m_fixedVValue, m_palette.fixedRectangle);
        drawValue(painter, {m_fixedRectangle.topLeft(), m_fixedRectangle.topRight()}, m_fixedHValue, m_palette.fixedRectangle);
    }
}

QRectF MainWindow::toFloat(const QRect& rectangle)
{
    float x = rectangle.x();
    float y = rectangle.y();
    float w = rectangle.width();
    float h = rectangle.height();
    return QRectF{x+ 0.5, y + 0.5, w , h};
}

QLineF MainWindow::toFloat(const QLine& line)
{
    float x1 = line.x1();
    float y1 = line.y1();
    float x2 = line.x2();
    float y2 = line.y2();
    return QLineF{x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5};
}

void MainWindow::drawValue(QPainter& painter, const QLine& line, int value, const QColor& color)
{
    QFontMetrics fm(font());
    auto text = QString::number(value);
    auto textH = fm.height();
    auto textShift{2};
    int x{0}, y{0}, textW;

    if (value != 0)
    {
        if (line.x1() == line.x2())
        {
            textW = fm.horizontalAdvance(text);
            x = line.x1() + textShift;
            y = line.center().y() + textH / 4;
            if (x + textW > rect().right())
            {
                x = qMin(line.x1(), rect().right()) - textW - textShift;
            }
        }
        else if (line.y1() == line.y2())
        {
            textW = fm.horizontalAdvance(text);
            x = line.center().x() - textW / 2;
            y = line.y1() - textShift;
            if (y < textH + textShift)
            {
                y = line.y1() + textH;
            }
            if (x + textW / 2 > rect().right())
            {
                x = rect().right() - textW - textShift;
            }
        }

        m_pen.setColor(color);
        painter.setPen(m_pen);
        painter.drawText(x, y, text);
    }
}

void MainWindow::draw()
{
    if (m_isActivated)
    {
        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing, false);

        painter.scale(m_scale, m_scale);
        painter.translate(-m_scaleShiftX, -m_scaleShiftY);

        drawBackground(painter);
        drawCursor(painter);
        drawRectangles(painter);
        drawMeasurer(painter);
        drawValues(painter);

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
