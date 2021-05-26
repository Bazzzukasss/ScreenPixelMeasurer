#ifndef DATA_H
#define DATA_H

#include <QColor>
#include <QPixmap>

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
    QPixmap screenImage;
    QPoint cursorPoint;
    QLine cursorHLine;
    QLine cursorVLine;
    QLine measureHLine;
    QLine measureVLine;
    QRect cursorRectangle;
    QRect windowRectangle;
    QRect fixedRectangle;
    QRect scaledRectangle;
    QRect measureRectangle;
    std::array<QLine, 4> fixedLines;
    int scale;
    bool isMeasurerRectPresent;
    bool isFixedRectVisible;
};

#endif // DATA_H
