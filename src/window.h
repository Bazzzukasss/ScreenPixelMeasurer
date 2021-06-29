#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "data.h"

class View;

class Window : public QMainWindow
{
    Q_OBJECT    

    const QString kTitle{"LMB - add/remove fixed rect; "
                         "Mouse Wheel - zooming; "
                         "RMB - panning; "
                         "P - switch palette; "
                         "Space - remove fixed rect"};
public:
    explicit Window(QWidget* parent = nullptr);

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;

private:
    View* m_view;
    QPoint m_lastWindowPos;

private:
    void initialize();
    void grabScreen();
    void updateTitle(const RenderData& renderData);
};

#endif // WINDOW_H
