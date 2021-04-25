#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QMainWindow>
#include <QPen>
#include <QSet>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct Palette {
        QColor border;
        QColor fixedRectangle;
        QColor cursorRectangle;
        QColor cursorLines;
        QColor measurerLines;
        QColor measurerText;
    };

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

private:
    QImage m_screenImage;
    QPoint m_cursorPoint;
    QLine m_cursorHLine;
    QLine m_cursorVLine;
    QLine m_measureHLine;
    QLine m_measureVLine;
    QRect m_cursorRectangle;
    QRect m_fixedRectangle;    
    int m_scale;
    int m_scaleShiftX;
    int m_scaleShiftY;
    bool m_isActivated;
    bool m_isFixedRectanglePresent;
    Palette m_palette;

    void grabScreen();
    void changeScale(const QPoint& delta);
    void calculateShifts();
    void calculateCursorRectangle(int x, int y);
    void calculateMeasureRectangle();
    void draw();
    void drawBackground(QPainter& painter);
    void drawMeasurer(QPainter& painter);
    void drawCursor(QPainter& painter);
    void drawRectangles(QPainter& painter);
    void drawValue(QPainter& painter, const QLine& line, int deltaValue, const QColor& color);
    void drawValues(QPainter& painter);
    int beamTo(int startPos, int endPos, int coord, int step,
               Qt::Orientation orientation, const QRgb& color);
};

#endif // CANVASWIDGET_H
