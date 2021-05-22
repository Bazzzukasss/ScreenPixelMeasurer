#ifndef PAINTER_H
#define PAINTER_H

#include <QPainter>
#include <QSet>

#include "data.h"

class Painter : public QPainter
{
public:
    Painter();
    explicit Painter(QPaintDevice* device);

    void draw(const RenderData& renderData);
    void setPalette(const Palette& palette);

private:
    void initialize();
    void drawBackground(const RenderData& renderData);
    void drawMeasurerLines(const RenderData& renderData);
    void drawCursorLines(const RenderData& renderData);
    void drawCursorRectangle(const RenderData& renderData);
    void drawFixedRectangle(const RenderData& renderData);
    void drawMeasureRectangle(const RenderData& renderData);
    void drawValues(const RenderData& renderData);
    void drawFixedLines(const RenderData& renderData);
    void drawMeasurerLine(const QLine& line, bool begTick, bool endTick);
    void drawValue(const QRect& rect, const QLine& line, int value, const QColor& color);
    void drawValue(const QRect& windowRect, const QRect& measureRect, const QColor& color);
    QRectF toFloat(const QRect& rectangle);
    QLineF toFloat(const QLine& line);
    bool isFixedRectanglePresent(const RenderData& renderData) const;
    bool isMeasureRectanglePresent(const RenderData& renderData) const;
    void applyPen(const QColor& color, Qt::PenStyle style);

private:
    QPen m_pen;
    Palette m_palette;
};

#endif // PAINTER_H
