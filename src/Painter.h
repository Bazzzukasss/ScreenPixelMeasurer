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
    QColor measurerText;
};

struct RenderData{
    QImage screenImage;
    QPoint cursorPoint;
    QPoint windowPos;
    QPoint referencePoint;
    QLine cursorHLine;
    QLine cursorVLine;
    QLine measureHLine;
    QLine measureVLine;
    QRect cursorRectangle;
    QRect windowRectangle;
    QList<QRect> referenceRectangles;
    int scale;
    int scaleShiftX;
    int scaleShiftY;
    bool isActivated;
    bool isReferencePointPresent;
};

class Painter : public QPainter
{
public:
    Painter();
    explicit Painter(QPaintDevice* device);

    void draw(const RenderData& renderData);

    void setPalette(const Palette& palette);
    Palette getPalette() const;

private:
    void initialize();
    void drawBackground(const RenderData& renderData);
    void drawMeasurer(const RenderData& renderData);
    void drawCursor(const RenderData& renderData);
    void drawRectangles(const RenderData& renderData);
    void drawValues(const RenderData& renderData);
    void drawReferencePoint(const RenderData& renderData);
    void drawMeasurerLine(const QLine& line);
    void drawValue(const QRect& rect, const QLine& line, int value, const QColor& color);
    QRectF toFloat(const QRect& rectangle);
    QLineF toFloat(const QLine& line);

private:
    QPen m_pen;
    Palette m_palette;
};

#endif // PAINTER_H
