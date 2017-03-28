#include <QCoreApplication>
#include "server.h"

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << QCoreApplication::libraryPaths();
    Server server;
    return a.exec();
}

