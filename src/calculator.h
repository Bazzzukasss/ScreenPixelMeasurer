#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QColor>
#include <QPixmap>

class Calculator
{
public:
    Calculator();

    static QRect calculateCursorRectangle(int x, int y, const QImage& img);
    static std::array<QLine, 2> calculateCursorLines(const QRect& cursorRect, int x, int y);
    static std::array<QLine, 4> calculateFixedLines(const QRect& fixedRect, int w, int h);
    static std::array<QLine, 2> calculateMeasureLines(const QRect& cursorRect, const QRect& fixedRect);

    static int beamTo(int startPos, int endPos, int coord, int step,
                      Qt::Orientation orientation, const QRgb& color, const QImage& img);
};

#endif // CALCULATOR_H
