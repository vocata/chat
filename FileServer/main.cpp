#include "fileserver.h"

#include <QCoreApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    FileServer fileServer;
    fileServer.start();
    return a.exec();
}
