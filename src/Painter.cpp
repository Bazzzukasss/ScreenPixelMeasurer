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
    if (renderData.isReferencePointPresent)
    {
        m_pen.setColor(m_palette.measurerLines);
        setPen(m_pen);
        if (abs(renderData.measureHLine.dx()) != 0)
        {
            drawMeasurerLine(renderData.measureHLine);
        }
        if (abs(renderData.measureVLine.dy()) != 0)
        {
            drawMeasurerLine(renderData.measureVLine);
        }
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

    for (auto referenceRectangle : renderData.referenceRectangles)
    {
        if (referenceRectangle != renderData.cursorRectangle)
        {
            m_pen.setColor(m_palette.fixedRectangle);
            setPen(m_pen);
            drawRect(toFloat(referenceRectangle));
        }
    }
}

void Painter::drawMeasurerLine(const QLine &line)
{
    auto vTick = line.dx() ? 2 : 0;
    auto hTick = line.dy() ? 2 : 0;

    m_pen.setStyle(Qt::DotLine);
    setPen(m_pen);

    drawLine(toFloat(line));

    m_pen.setStyle(Qt::SolidLine);
    setPen(m_pen);

    drawLine(toFloat(QLine{line.x1() - hTick, line.y1() - vTick,
                                   line.x1() + hTick, line.y1() + vTick}));
    drawLine(toFloat(QLine{line.x2() - hTick, line.y2() - vTick,
                                   line.x2() + hTick, line.y2() + vTick}));
}

void Painter::drawValues(const RenderData& renderData)
{
    auto rect = renderData.windowRectangle;
    auto vLine = QLine{renderData.cursorRectangle.bottomRight(), renderData.cursorRectangle.topRight()};
    auto hLine = QLine{renderData.cursorRectangle.topLeft(), renderData.cursorRectangle.topRight()};

    drawValue(rect, renderData.measureHLine, abs(renderData.measureHLine.dx()) + 1, m_palette.measurerLines);
    drawValue(rect, renderData.measureVLine, abs(renderData.measureVLine.dy()) + 1, m_palette.measurerLines);

    drawValue(rect, vLine, renderData.cursorRectangle.width() + 1, m_palette.cursorRectangle);
    drawValue(rect, hLine, renderData.cursorRectangle.height() + 1, m_palette.cursorRectangle);

    for (auto referenceRectangle : renderData.referenceRectangles)
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
    if (renderData.isReferencePointPresent)
    {
        m_pen.setColor(m_palette.fixedRectangle);
        setPen(m_pen);
        auto rect = renderData.windowRectangle;
        drawLine(toFloat(QLine{renderData.referencePoint.x(), 0, renderData.referencePoint.x(), rect.height() - 1}));
        drawLine(toFloat(QLine{0, renderData.referencePoint.y(), rect.width() - 1, renderData.referencePoint.y()}));
    }
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

Palette Painter::getPalette() const
{
    return m_palette;
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
