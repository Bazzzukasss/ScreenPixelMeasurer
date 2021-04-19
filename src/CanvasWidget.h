#ifndef CANVASWIDGET_H
#define CANVASWIDGET_H

#include <QWidget>
#include <QPen>

class CanvasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CanvasWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    QImage m_screenImage;
    bool m_isActivated;
    QLine m_hLine;
    QLine m_vLine;
    QPen m_pen;

    void grabScreen();
    void drawMeasurer();
    void calculateMeasurer();
};

#endif // CANVASWIDGET_H
