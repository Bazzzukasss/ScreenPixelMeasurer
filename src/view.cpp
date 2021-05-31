#include <QMouseEvent>

#include "view.h"

View::View(QWidget* parent)
    : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
}

void View::mousePressEvent(QMouseEvent* event)
{
    emit mousePressed(event);
    event->accept();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event);
    event->accept();
}

void View::wheelEvent(QWheelEvent* event)
{
    emit mouseScrolled(event);
    event->accept();
}
