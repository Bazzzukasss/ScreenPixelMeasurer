#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QMainWindow>
#include <QPen>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QImage m_screenImage;
    QPoint m_centerPoint;
    QLine m_centerHLine;
    QLine m_centerVLine;
    QRect m_rectangle;
    bool m_isActivated;

    void grabScreen();
    void draw();
    void calculateMeasurer(int cx, int cy);
    void drawRuller(QPainter& painter);
    void drawMeasurer(QPainter& painter);
    int measTo(int startPos, int endPos, int coord, int step,
               Qt::Orientation orientation, const QRgb& color);
};

#endif // CANVASWIDGET_H
