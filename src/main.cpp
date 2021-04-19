#include <QApplication>

#include "CanvasWidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CanvasWidget w;
    w.showFullScreen();

    return a.exec();
}
