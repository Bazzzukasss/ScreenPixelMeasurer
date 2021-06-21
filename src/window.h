#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

class View;

class Window : public QMainWindow
{
    Q_OBJECT    

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
};

#endif // WINDOW_H
