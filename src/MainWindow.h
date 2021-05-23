#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPen>
#include <QSet>

#include "data.h"
#include "view.h"
#include "scene.h"


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
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    RenderData m_renderData;
    int m_paletteIndex{0};
    QPoint m_lastWindowPos;
    QPoint m_lastMousePos;
    QVector<Palette> m_palettes{kDarkPalette, kLightPalette};
    View* m_view;
    Scene* m_scene;

    void initialize();
    void grabScreen();
    void setFixedRectangle();
    void clearFixedRectangle();
    void changeScale(const QPoint& delta);
    void switchPalette();
    void adjust(int dx, int dy);
    void calculate();
    int beamTo(int startPos, int endPos, int coord, int step,
               Qt::Orientation orientation, const QRgb& color);
private:
    void onMouseMove(QMouseEvent* event);
    void onMousePress(QMouseEvent* event);
    void onMouseRelease(QMouseEvent* event);
    void onMouseScroll(QWheelEvent* event);

};

#endif // MAINWINDOW_H
