#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "../clamlog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();


    cInfo() << 333;
    cFatal() << 999;
    cDebug() << "AAA";

    int quit_ = a.exec();
    cDebug() << QObject::tr("program exits: %1").arg(quit_);
    Clam::Log::instance().quit();
    return quit_;
}
