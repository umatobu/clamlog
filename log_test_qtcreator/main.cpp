#include <QApplication>
#include "mainwindow.h"
#include "../clamlog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    cInfo() << 333;
    cFatal() << 999;
    cDebug() << "AAA";

    MainWindow w;
    w.show();

    int quit_ = a.exec();
    cDebug() << QObject::tr("program exits: %1").arg(quit_);
    Clam::Log::instance().quit();
    return quit_;
}
