#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QMainWindow>
#include <QPen>
#include <QSet>

#include "Painter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    const int kMinScale{1};
    const int kMaxScale{8};

    const Palette kDarkPalette {
        Qt::white,
        Qt::magenta,
        Qt::cyan,
        Qt::darkCyan,
        Qt::yellow,
        Qt::yellow,
    };

    const Palette kLightPalette {
        Qt::black,
        Qt::red,
        Qt::blue,
        Qt::darkBlue,
        Qt::green,
        Qt::green,
    };

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    RenderData m_renderData;
    Painter m_painter;
    int m_paletteIndex{0};
    QVector<Palette> m_palettes{kDarkPalette, kLightPalette};

    void initialize();
    void grabScreen();
    void setReferenceRectangle();
    void setReferencePoint(int x, int y);
    void changeScale(const QPoint& delta);
    void calculateShifts();
    void calculateCursorRectangle(int x, int y);
    void calculateMeasurer();
    int calculateScaledX(int x);
    int calculateScaledY(int y);
    int beamTo(int startPos, int endPos, int coord, int step,
               Qt::Orientation orientation, const QRgb& color);
    void switchPalette();
};

#endif // CANVASWIDGET_H
