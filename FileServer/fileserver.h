#ifndef FILESERVER_H
#define FILESERVER_H

#include <QTcpServer>

#define FILE_SERVER_PORT 7789

class FileServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit FileServer(QObject *parent = 0);
    void start();
protected:
    void incomingConnection(qintptr handle);
};

#endif // FILESERVER_H
