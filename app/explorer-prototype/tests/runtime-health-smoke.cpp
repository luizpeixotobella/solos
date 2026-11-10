#include "runtimehealth.h"
#include <QCoreApplication>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    if (argc != 2) return 2;
    return runtimeSnapshotAvailable(QString::fromLocal8Bit(argv[1]), 200) ? 0 : 1;
}
