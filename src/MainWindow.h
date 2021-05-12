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
        QColor{0x333333},           //background
        Qt::white,                  //border;
        QColor{0xE4A8F9},           //fixedRectangle;
        QColor{0x99ea86},           //fixedLines;
        Qt::cyan,                   //cursorRectangle;
        Qt::darkCyan,               //cursorLines;
        Qt::yellow,                 //measurerLines;
        8.0
    };

    const Palette kLightPalette {
        QColor{0xcccccc},
        Qt::black,
        Qt::magenta,
        Qt::darkGreen,
        Qt::darkBlue,
        Qt::blue,
        Qt::red,
        8.0
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
    QPoint m_lastMousePos;
    QVector<Palette> m_palettes{kDarkPalette, kLightPalette};

    void initialize();
    void grabScreen();
    void setFixedRectangle();
    void clearFixedRectangle();
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
