#include <QCoreApplication>
#include "../log.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    cInfo() << 333;
    cFatal() << 999;
    cDebug() << "AAA";

    return a.exec();
}
