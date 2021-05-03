#ifndef PAINTER_H
#define PAINTER_H

#include <QPainter>
#include <QSet>

struct Palette {
    QColor border;
    QColor fixedRectangle;
    QColor cursorRectangle;
    QColor cursorLines;
    QColor measurerLines;
    QColor referenceLines;
    QColor measurerText;
};

struct RenderData{
    QImage screenImage;
    QPoint referencePoint;
    QPoint cursorPoint;
    QLine cursorHLine;
    QLine cursorVLine;
    QLine measureHLine;
    QLine measureVLine;
    QLine referenceHLine;
    QLine referenceVLine;
    QRect cursorRectangle;
    QRect windowRectangle;
    QList<QRect> fixedRectangles;
    int scale;
    int scaleShiftX;
    int scaleShiftY;
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
    void drawMeasurer(const RenderData& renderData);
    void drawCursor(const RenderData& renderData);
    void drawRectangles(const RenderData& renderData);
    void drawValues(const RenderData& renderData);
    void drawReferencePoint(const RenderData& renderData);
    void drawMeasurerLine(const QLine& line, bool begTick, bool endTick);
    void drawValue(const QRect& rect, const QLine& line, int value, const QColor& color);
    QRectF toFloat(const QRect& rectangle);
    QLineF toFloat(const QLine& line);

private:
    QPen m_pen;
    Palette m_palette;
};

#endif // PAINTER_H
