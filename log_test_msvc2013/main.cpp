#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "../log.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();


    uInfo() << 333;
    uFatal() << 999;

    return a.exec();
}
