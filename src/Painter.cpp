#include "Painter.h"

Painter::Painter()
{
    initialize();
}

Painter::Painter(QPaintDevice* device) :
    QPainter(device)
{
    initialize();
}

void Painter::initialize()
{
    m_pen.setJoinStyle(Qt::MiterJoin);
    m_pen.setWidth(1);
}

void Painter::drawBackground(const RenderData& renderData)
{
    m_pen.setColor(m_palette.border);
    setPen(m_pen);
    auto rect = renderData.windowRectangle;
    drawImage(rect, renderData.screenImage);
    drawRect(toFloat(QRect{0, 0, rect.width() - 1, rect.height() - 1}));
}

void Painter::drawMeasurer(const RenderData& renderData)
{
    m_pen.setColor(m_palette.measurerLines);
    if (abs(renderData.measureHLine.dx()) != 0)
    {
        m_pen.setStyle(Qt::CustomDashLine);
        m_pen.setDashPattern({1, 4});
        setPen(m_pen);

        drawMeasurerLine(renderData.measureHLine, false, true);
    }
    if (abs(renderData.measureVLine.dy()) != 0)
    {
        m_pen.setStyle(Qt::CustomDashLine);
        m_pen.setDashPattern({1, 4});
        setPen(m_pen);

        drawMeasurerLine(renderData.measureVLine, false, true);
    }
    if (abs(renderData.referenceHLine.dx()) != 0)
    {
        m_pen.setStyle(Qt::DashDotLine);
        setPen(m_pen);

        drawMeasurerLine(renderData.referenceHLine, false, true);
    }
    if (abs(renderData.referenceVLine.dy()) != 0)
    {
        m_pen.setStyle(Qt::DashDotLine);
        setPen(m_pen);

        drawMeasurerLine(renderData.referenceVLine, false, true);
    }
}

void Painter::drawCursor(const RenderData& renderData)
{
    m_pen.setColor(m_palette.cursorLines);
    setPen(m_pen);
    drawLine(toFloat(renderData.cursorHLine));
    drawLine(toFloat(renderData.cursorVLine));
}

void Painter::drawRectangles(const RenderData& renderData)
{
    m_pen.setColor(m_palette.cursorRectangle);
    setPen(m_pen);
    drawRect(toFloat(renderData.cursorRectangle));

    for (auto referenceRectangle : renderData.fixedRectangles)
    {
        if (referenceRectangle != renderData.cursorRectangle)
        {
            m_pen.setColor(m_palette.fixedRectangle);
            setPen(m_pen);
            drawRect(toFloat(referenceRectangle));
        }
    }
}

void Painter::drawMeasurerLine(const QLine &line, bool begTick, bool endTick)
{
    auto vTick = line.dx() ? 2 : 0;
    auto hTick = line.dy() ? 2 : 0;

    drawLine(toFloat(line));

    m_pen.setStyle(Qt::SolidLine);
    setPen(m_pen);

    if (begTick)
    {
        drawLine(toFloat(QLine{line.x1() - hTick, line.y1() - vTick,
                                       line.x1() + hTick, line.y1() + vTick}));
    }

    if (endTick)
    {
        drawLine(toFloat(QLine{line.x2() - hTick, line.y2() - vTick,
                                       line.x2() + hTick, line.y2() + vTick}));
    }
}

void Painter::drawValues(const RenderData& renderData)
{
    auto rect = renderData.windowRectangle;
    auto vLine = QLine{renderData.cursorRectangle.bottomRight(), renderData.cursorRectangle.topRight()};
    auto hLine = QLine{renderData.cursorRectangle.topLeft(), renderData.cursorRectangle.topRight()};

    drawValue(rect, renderData.measureHLine, abs(renderData.measureHLine.dx()), m_palette.measurerLines);
    drawValue(rect, renderData.measureVLine, abs(renderData.measureVLine.dy()), m_palette.measurerLines);

    drawValue(rect, renderData.referenceHLine, abs(renderData.referenceHLine.dx()), m_palette.measurerLines);
    drawValue(rect, renderData.referenceVLine, abs(renderData.referenceVLine.dy()), m_palette.measurerLines);

    drawValue(rect, vLine, renderData.cursorRectangle.height() + 1, m_palette.cursorRectangle);
    drawValue(rect, hLine, renderData.cursorRectangle.width() + 1, m_palette.cursorRectangle);

    for (auto referenceRectangle : renderData.fixedRectangles)
    {
        if (referenceRectangle != renderData.cursorRectangle)
        {
            vLine = QLine{referenceRectangle.bottomRight(), referenceRectangle.topRight()};
            hLine = QLine{referenceRectangle.topLeft(), referenceRectangle.topRight()};

            drawValue(rect, vLine, referenceRectangle.height() + 1, m_palette.fixedRectangle);
            drawValue(rect, hLine, referenceRectangle.width() + 1, m_palette.fixedRectangle);
        }
    }
}

void Painter::drawReferencePoint(const RenderData& renderData)
{
    auto rect = renderData.windowRectangle;
    auto x = renderData.referencePoint.x();
    auto y = renderData.referencePoint.y();

    m_pen.setStyle(Qt::DashDotLine);
    m_pen.setColor(m_palette.referenceLines);
    setPen(m_pen);

    drawLine(toFloat(QLine{x, y, rect.right(), y}));
    drawLine(toFloat(QLine{x, y, rect.left(), y}));
    drawLine(toFloat(QLine{x, y, x, rect.top()}));
    drawLine(toFloat(QLine{x, y, x, rect.bottom()}));
}

QRectF Painter::toFloat(const QRect& rectangle)
{
    float x = rectangle.x();
    float y = rectangle.y();
    float w = rectangle.width();
    float h = rectangle.height();
    return QRectF{x+ 0.5, y + 0.5, w , h};
}

QLineF Painter::toFloat(const QLine& line)
{
    float x1 = line.x1();
    float y1 = line.y1();
    float x2 = line.x2();
    float y2 = line.y2();
    return QLineF{x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5};
}

void Painter::setPalette(const Palette& palette)
{
    m_palette = palette;
}

void Painter::drawValue(const QRect& rect, const QLine& line, int value, const QColor& color)
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
            if (x + textW > rect.right())
            {
                x = qMin(line.x1(), rect.right()) - textW - textShift;
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
            if (x + textW / 2 > rect.right())
            {
                x = rect.right() - textW - textShift;
            }
        }

        m_pen.setColor(color);
        setPen(m_pen);
        drawText(x, y, text);
    }
}

void Painter::draw(const RenderData& renderData)
{
    if (renderData.isActivated)
    {
        setRenderHint(QPainter::Antialiasing, false);

        scale(renderData.scale, renderData.scale);
        translate(-renderData.scaleShiftX, -renderData.scaleShiftY);

        drawBackground(renderData);
        drawCursor(renderData);
        drawRectangles(renderData);
        drawReferencePoint(renderData);
        drawMeasurer(renderData);
        drawValues(renderData);
    }
}
