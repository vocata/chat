#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QFile>
#include <QTcpSocket>

class QTimer;
class QFile;
class FileHandler : public QTcpSocket
{
    Q_OBJECT
enum TransferType: qint32 { Download = 100, Upload };

public:
    explicit FileHandler(QObject *parent = 0);
    void start(qintptr connfd);
private:
    qint32 m_preDataTotalBytes;
    qint32 m_preDataBytesReceived;
    QByteArray m_preData;
    qint64 m_fileDataTotalBytes;
    qint64 m_fileDataBytesReceived;
    qint64 m_fileDataBytesSend;
    qint64 m_fileDataBytesToSend;
    QFile m_localFile;

    void m_putData(QByteArray json);
    void m_putDataAck();                //发送数据大小
    void m_getData(QByteArray json);
    void m_getDataAck();                //发送服务器状态
private slots:
    void m_selectTransferType();
    void m_putDataHandler(qint64 size);
    void m_downloadCloseHandler();
    void m_getDataHandler();
    void m_uploadCloseHandler();              //处理意外情况
    void m_errorHandler(QAbstractSocket::SocketError error);
};

const qint64 PayloadSize = 64 * 1024;

#endif // FILEHANDLER_H
