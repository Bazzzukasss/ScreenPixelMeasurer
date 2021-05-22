#include "Painter.h"
#include <QDebug>

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
    applyPen(m_palette.border, Qt::SolidLine);
    drawPixmap(renderData.windowRectangle, renderData.screenImage);
    drawRect(toFloat(renderData.scaledRectangle));
}

void Painter::drawMeasurerLines(const RenderData& renderData)
{
    if (isFixedRectanglePresent(renderData))
    {
        if (renderData.measureHLine.dx() > 0)
        {
            drawMeasurerLine(renderData.measureHLine, true, true);
        }
        if (renderData.measureVLine.dy() > 0)
        {
            drawMeasurerLine(renderData.measureVLine, true, true);
        }
    }
}

void Painter::drawCursorLines(const RenderData& renderData)
{
    applyPen(m_palette.cursorLines, Qt::SolidLine);
    drawLine(toFloat(renderData.cursorHLine));
    drawLine(toFloat(renderData.cursorVLine));
}

void Painter::drawCursorRectangle(const RenderData& renderData)
{
    applyPen(m_palette.cursorRectangle, Qt::SolidLine);
    drawRect(toFloat(renderData.cursorRectangle));
}

void Painter::drawFixedRectangle(const RenderData& renderData)
{
    if (isFixedRectanglePresent(renderData))
    {
        applyPen(m_palette.fixedRectangle, Qt::SolidLine);
        drawRect(toFloat(renderData.fixedRectangle));
    }
}

void Painter::drawMeasureRectangle(const RenderData& renderData)
{
    if (isMeasureRectanglePresent(renderData))
    {
        applyPen(m_palette.cursorRectangle, Qt::SolidLine);
        drawRect(toFloat(renderData.measureRectangle));
    }
}

void Painter::drawMeasurerLine(const QLine &line, bool begTick, bool endTick)
{
    auto vTick = line.dx() ? 2 : 0;
    auto hTick = line.dy() ? 2 : 0;

    applyPen(m_palette.measurerLines, Qt::DotLine);
    drawLine(toFloat(line));

    applyPen(m_palette.measurerLines, Qt::SolidLine);
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

void Painter::drawValue(const QRect& windowRect, const QRect& measureRect, const QColor& color)
{
    auto vLine = QLine{measureRect.bottomRight(), measureRect.topRight()};
    auto hLine = QLine{measureRect.topLeft(), measureRect.topRight()};
    drawValue(windowRect, vLine, abs(measureRect.height()) + 1, color);
    drawValue(windowRect, hLine, abs(measureRect.width()) + 1, color);
}

void Painter::drawValues(const RenderData& renderData)
{
    auto fsize = m_palette.fontPointSize / renderData.scale;
    auto fnt = font();
    fnt.setBold(true);
    fnt.setPointSizeF(fsize + 2.0);
    setFont(fnt);

    auto rect = renderData.scaledRectangle;

    if (renderData.isMeasurerMode)
    {
        if (isMeasureRectanglePresent(renderData))
        {
            drawValue(rect, renderData.measureRectangle, m_palette.cursorRectangle);
        }
    }
    else
    {
        drawValue(rect, renderData.cursorRectangle, m_palette.cursorRectangle);

        if (isFixedRectanglePresent(renderData))
        {
            drawValue(rect, renderData.fixedRectangle, m_palette.fixedRectangle);

            fnt.setPointSizeF(fsize + 0.5);
            setFont(fnt);
            drawValue(rect, renderData.measureHLine, renderData.measureHLine.dx() + 1, m_palette.measurerLines);
            drawValue(rect, renderData.measureVLine, renderData.measureVLine.dy() + 1, m_palette.measurerLines);
        }
    }
}

void Painter::drawFixedLines(const RenderData& renderData)
{
    if (isFixedRectanglePresent(renderData))
    {
        applyPen(m_palette.fixedLines, Qt::DashLine);

        for (auto line : renderData.fixedLines)
        {
            drawLine(toFloat(line));
        }
    }
}

QRectF Painter::toFloat(const QRect& rectangle)
{
    float x = rectangle.x();
    float y = rectangle.y();
    float w = rectangle.width();
    float h = rectangle.height();
    return QRectF{x + 0.5, y + 0.5, w , h};
}

QLineF Painter::toFloat(const QLine& line)
{
    float x1 = line.x1();
    float y1 = line.y1();
    float x2 = line.x2();
    float y2 = line.y2();
    return QLineF{x1 + 0.5, y1 + 0.5, x2 + 0.5, y2 + 0.5};
}

bool Painter::isFixedRectanglePresent(const RenderData& renderData) const
{
    return renderData.fixedRectangle.height() != 0 &&
            renderData.fixedRectangle.width() != 0;
}

bool Painter::isMeasureRectanglePresent(const RenderData &renderData) const
{
    return renderData.measureRectangle.bottomLeft() !=
           renderData.measureRectangle.topRight();
}

void Painter::applyPen(const QColor& color, Qt::PenStyle style)
{
    m_pen.setColor(color);
    m_pen.setStyle(style);
    setPen(m_pen);
}

void Painter::setPalette(const Palette& palette)
{
    m_palette = palette;
}

void Painter::drawValue(const QRect& rect, const QLine& line, int value, const QColor& color)
{
    auto fm = fontMetrics();
    auto text = QString::number(value);
    int x{0}, y{0};
    QRect textRect = fm.boundingRect(text).marginsAdded({1, 0, 1, 0});
    int textW = textRect.width();
    auto textH = textRect.height();

    if (value != 0)
    {
        if (line.x1() == line.x2())
        {
            x = line.x1() + 3;
            y = line.center().y() + textH / 2;
        }
        else if (line.y1() == line.y2())
        {
            x = line.center().x() - textW / 2;
            y = line.y1();
        }

        if (x + textW > rect.right())
        {
            x = rect.right() - textW;
        }

        if (x < rect.left())
        {
            x = rect.left();
        }

        if (y - textH < rect.top())
        {
            y = rect.top() + textH;
        }

        if (y > rect.bottom())
        {
            y = rect.bottom();
        }

        textRect.moveTo(x, y - textH);
        applyPen(color, Qt::SolidLine);

        fillRect(textRect, m_palette.background);
        drawText(textRect, Qt::AlignCenter, text);
    }
}

void Painter::draw(const RenderData& renderData)
{
    if (renderData.isActivated)
    {
        setRenderHint(QPainter::Antialiasing, false);

        scale(renderData.scale, renderData.scale);
        translate(-(renderData.scaleShiftX + renderData.centerShiftX),
                  -(renderData.scaleShiftY + renderData.centerShiftY));

        drawBackground(renderData);
        if (renderData.isMeasurerMode)
        {
            drawMeasureRectangle(renderData);
        }
        else
        {
            drawCursorLines(renderData);
            drawFixedLines(renderData);
            drawFixedRectangle(renderData);
            drawCursorRectangle(renderData);
            drawMeasurerLines(renderData);
        }

        drawValues(renderData);
    }
}
