#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
        Qt::white,                  //border;
        QColor{0xff, 0xA5, 0x00},   //fixedRectangle;
        Qt::cyan,                   //cursorRectangle;
        Qt::darkCyan,               //cursorLines;
        Qt::yellow,                 //measurerLines;
        QColor{0xf6, 0x38, 0xdc},   //referenceLines;
    };

    const Palette kLightPalette {
        Qt::black,
        QColor{0x60, 0x20, 0x80},
        Qt::blue,
        QColor(Qt::blue).lighter(),
        QColor{0xe1, 0x64, 0x28},
        Qt::magenta,
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
    QPoint m_lastWindowPos;
    QVector<Palette> m_palettes{kDarkPalette, kLightPalette};

    void initialize();
    void grabScreen();
    void setReferenceRectangle();
    void changeScale(const QPoint& delta);
    void switchPalette();
    void adjust(int dx, int dy);
    void calculate();
    void calculateShifts();
    int calculateScaledX(int x);
    int calculateScaledY(int y);
    int beamTo(int startPos, int endPos, int coord, int step,
               Qt::Orientation orientation, const QRgb& color);
};

#endif // MAINWINDOW_H
