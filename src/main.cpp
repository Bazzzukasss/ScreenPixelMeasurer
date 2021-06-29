#include <QApplication>

#include "window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Window w;
    w.resize(1024, 800);
    w.show();

    return a.exec();
}
