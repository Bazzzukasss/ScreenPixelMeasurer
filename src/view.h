#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include "scene.h"

class View : public QGraphicsView
{
    Q_OBJECT

    const QPoint kPoint{1,1};
    const int kMinScale{1};
    const int kMaxScale{8};

    const Palette kDarkPalette {
        QColor{0x333333},           //background
        QColor{0xE4A8F9},           //fixedRectangle;
        QColor{0x99ea86},           //fixedLines;
        Qt::cyan,                   //cursorRectangle;
        Qt::darkCyan,               //cursorLines;
        Qt::yellow,                 //measurerLines;
    };

    const Palette kLightPalette {
        QColor{0xcccccc},
        Qt::magenta,
        Qt::darkGreen,
        Qt::darkBlue,
        Qt::blue,
        Qt::red,
    };

public:
    View(QWidget* parent = nullptr);

    void switchPalette();
    void shiftScene(int dx, int dy);
    void setPixmap(const QPixmap& pixmap);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    Scene* m_scene;
    RenderData m_renderData;
    QPoint m_lastMousePos;
    QVector<Palette> m_palettes{kDarkPalette, kLightPalette};
    int m_scale{kMinScale};
    int m_paletteIndex{0};

private:
    void updateScene();
    void setFixedRectangle();
    void correctFixedRectangle(const QRect& rect);
    void changeScale(const QPoint& delta);
    void calculate();
    int beamTo(int startPos, int endPos, int coord, int step,
               Qt::Orientation orientation, const QRgb& color);
};

#endif // VIEW_H
