#ifndef PAINTER_H
#define PAINTER_H

#include <QPainter>
#include <QSet>

struct Palette {
    QColor background;
    QColor border;
    QColor fixedRectangle;
    QColor fixedLines;
    QColor cursorRectangle;
    QColor cursorLines;
    QColor measurerLines;
    float fontPointSize;
};

struct RenderData{
    QImage screenImage;
    QPoint cursorPoint;
    QLine cursorHLine;
    QLine cursorVLine;
    QLine measureHLine;
    QLine measureVLine;
    QRect cursorRectangle;
    QRect windowRectangle;
    QRect fixedRectangle;
    std::array<QLine, 4> fixedLines;
    int scale;
    int scaleShiftX;
    int scaleShiftY;
    int centerShiftX;
    int centerShiftY;
    bool isActivated;
};

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
    void drawRectangles(const RenderData& renderData);
    void drawValues(const RenderData& renderData);
    void drawFixedLines(const RenderData& renderData);
    void drawMeasurerLine(const QLine& line, bool begTick, bool endTick);
    void drawValue(const QRect& rect, const QLine& line, int value, const QColor& color);
    QRectF toFloat(const QRect& rectangle);
    QLineF toFloat(const QLine& line);
    bool isFixedRectanglePresent(const RenderData& renderData) const;
    void applyPen(const QColor& color, Qt::PenStyle style);

private:
    QPen m_pen;
    Palette m_palette;
};

#endif // PAINTER_H
