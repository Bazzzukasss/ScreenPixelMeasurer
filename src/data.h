#ifndef DATA_H
#define DATA_H

#include <QColor>
#include <QPixmap>

struct Palette {
    QColor background;
    QColor fixedRectangle;
    QColor fixedLines;
    QColor cursorRectangle;
    QColor cursorLines;
    QColor measureLines;
};

struct RenderData{
    QPixmap screenImage;
    QPoint cursorPoint;
    QLine cursorHLine;
    QLine cursorVLine;
    QLine measureHLine;
    QLine measureVLine;
    QRect cursorRectangle;
    QRect fixedRectangle;
    std::array<QLine, 4> fixedLines;
    bool isMeasurerRectPresent{false};
    bool isCursorRectPresent{false};
    bool isFixedRectPresent{false};
    bool isItemDragging{false};
};

#endif // DATA_H
