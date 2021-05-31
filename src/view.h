#ifndef VIEW_H
#define VIEW_H

#include <QGraphicsView>
#include "scene.h"

class View : public QGraphicsView
{
    Q_OBJECT

public:
    View(Scene* viewScene, QWidget* parent = nullptr);

signals:
    void mouseMoved(QMouseEvent* event);
    void mousePressed(QMouseEvent* event);
    void mouseScrolled(QWheelEvent* event);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    Scene* m_scene;
};

#endif // VIEW_H
