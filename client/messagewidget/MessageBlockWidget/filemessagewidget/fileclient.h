#ifndef FILECLIENT_H
#define FILECLIENT_H

#include <QTcpSocket>
#include <QFile>

enum TransferType: qint32 { Download = 100, Upload };

class Downloader : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent);
    explicit Downloader(const QString &hostName, quint16 port, QObject *parent = 0);
    void setAddress(const QString &hostName, quint16 port);
    void get(const QString &localFileName, const QString &remoteFileName);
    void cancel();

    ~Downloader();
signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void fileNotExists();
    void downloadFinish();
    void transerCancel();
private:
    QString m_hostName;
    quint16 m_port;
    QFile m_file;
    QByteArray m_jsonData;
    qint64 m_totalBytes;
    qint64 m_bytesReceived;
private slots:
    void m_hasConnected();
    void m_getDataHandler();
    void m_errorHandler(QAbstractSocket::SocketError error);
};

class Uploader : public QTcpSocket
{
    Q_OBJECT
public:
    explicit Uploader(QObject *parent);
    explicit Uploader(const QString &hostName, quint16 port, QObject *parent = 0);
    void setAddress(const QString &hostName, quint16 port);
    void put(const QString &localFileName, const QString &remoteFileName);
    void cancel();

    ~Uploader();
signals:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void uploadFinish();
    void transerCancel();
private:
    QString m_hostName;
    quint16 m_port;
    QFile m_file;
    QByteArray m_jsonData;
    qint64 m_totalBytes;
    qint64 m_bytesSend;
    qint64 m_bytesToSend;
    void m_sendSize();
private slots:
    void m_hasConnected();
    void m_reply();
    void m_putDataHandler(qint64 size);
    void m_errorHandler(QAbstractSocket::SocketError error);
};

const qint64 PayloadSize = 64 * 1024;

#endif // FILECLIENT_H
