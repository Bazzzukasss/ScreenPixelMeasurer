#include <QMouseEvent>

#include "view.h"
#include "scene.h"

View::View(Scene* viewScene, QWidget* parent)
    : QGraphicsView(parent)
    , m_scene(viewScene)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
    setScene(viewScene);
}

void View::mousePressEvent(QMouseEvent* event)
{
    if (!m_scene->isHoveredItemPresent())
    {
        emit mousePressed(event);
    }

    QGraphicsView::mousePressEvent(event);
    event->accept();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_scene->isHoveredItemPresent())
    {
        emit mouseMoved(event);
    }

    QGraphicsView::mouseMoveEvent(event);
    event->accept();
}

void View::wheelEvent(QWheelEvent* event)
{
    emit mouseScrolled(event);
    QGraphicsView::wheelEvent(event);
    event->accept();
}
