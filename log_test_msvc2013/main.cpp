#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "../log.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();


    cInfo() << 333;
    cFatal() << 999;
    cDebug() << "AAA";

    return a.exec();
}
