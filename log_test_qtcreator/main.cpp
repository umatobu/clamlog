#include <QCoreApplication>
#include "../log.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    uInfo() << 333;
    uFatal() << 999;

    return a.exec();
}
