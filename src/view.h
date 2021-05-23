#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(QWidget* parent = nullptr);

signals:
    void mouseMoved(QMouseEvent* event);
    void mousePressed(QMouseEvent* event);
    void mouseReleaseed(QMouseEvent* event);
    void mouseScrolled(QWheelEvent* event);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // VIEW_H
