#include "fileclient.h"
#include <QDataStream>
#include <QJsonDocument>

#include <QDebug>

Downloader::Downloader(QObject *parent)
    : QTcpSocket(parent)
{
    m_totalBytes = 0;
    m_bytesReceived = 0;
}

Downloader::Downloader(const QString &hostName, quint16 port, QObject *parent)
    : QTcpSocket(parent), m_hostName(hostName), m_port(port)
{
    m_totalBytes = 0;
    m_bytesReceived = 0;
}

void Downloader::setAddress(const QString &hostName, quint16 port)
{
    m_hostName = hostName;
    m_port = port;
}

void Downloader::get(const QString &localFileName, const QString &remoteFileName)
{
    m_file.setFileName(localFileName);
    if(!m_file.open(QIODevice::WriteOnly)) {
        qDebug() << "server: open file error!";
        return;
    }
    QString filePath = remoteFileName.section('/', 0, -2);
    QString fileName = remoteFileName.section('/', -1, -1);
    qDebug() << filePath << fileName;
    QVariantMap map;
    map.insert("filePath", filePath);
    map.insert("fileName", fileName);
    QJsonDocument doc = QJsonDocument::fromVariant(map);

    m_jsonData.clear();
    QDataStream json(&m_jsonData, QIODevice::WriteOnly);
    json.setVersion(QDataStream::Qt_5_4);
    json << qint32(0) << Download << doc.toJson();
    qint32 size = m_jsonData.size();
    json.device()->seek(0);
    json << size;

    connect(this, &Downloader::connected, this, &Downloader::m_hasConnected);
    connect(this, &Downloader::readyRead, this, &Downloader::m_getDataHandler);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));

    this->connectToHost(m_hostName, m_port);
}

void Downloader::cancel()
{
    if(m_bytesReceived != m_totalBytes)
        m_file.remove();
    m_totalBytes = 0;
    m_bytesReceived = 0;
    m_file.close();

    disconnect(this, &Downloader::connected, this, &Downloader::m_hasConnected);
    disconnect(this, &Downloader::readyRead, this, &Downloader::m_getDataHandler);
    disconnect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
    this->disconnectFromHost();
    emit transerCancel();
}

Downloader::~Downloader()
{
//    this->cancel();
}

void Downloader::m_hasConnected()
{
    this->write(m_jsonData);
}

void Downloader::m_getDataHandler()
{
    if(m_bytesReceived < (qint64)sizeof(qint64)) {
        if(this->bytesAvailable() < (qint64)sizeof(qint64))
            return;
        QDataStream in(this);
        in.setVersion(QDataStream::Qt_5_4);
        in >> m_totalBytes;
        m_bytesReceived += sizeof(qint64);
        if(m_bytesReceived == m_totalBytes)
            emit fileNotExists();
    }
    if(m_bytesReceived < m_totalBytes) {
        QByteArray fileBlock;
        fileBlock = this->read(m_totalBytes - m_bytesReceived);
        m_file.write(fileBlock);
        m_bytesReceived += fileBlock.size();
    }
    if(m_bytesReceived == m_totalBytes) {
        this->disconnectFromHost();
        m_file.close();
        emit downloadFinish();
    }
    emit downloadProgress(m_bytesReceived, m_totalBytes);
}

void Downloader::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;

    this->cancel();
}









Uploader::Uploader(QObject *parent)
    : QTcpSocket(parent)
{
    m_totalBytes = 0;
    m_bytesSend = 0;
    m_bytesToSend = 0;
}

Uploader::Uploader(const QString &hostName, quint16 port, QObject *parent)
    : QTcpSocket(parent), m_hostName(hostName), m_port(port)
{
    m_totalBytes = 0;
    m_bytesSend = 0;
    m_bytesToSend = 0;
}

void Uploader::setAddress(const QString &hostName, quint16 port)
{
    m_hostName = hostName;
    m_port = port;
}

void Uploader::put(const QString &localFileName, const QString &remoteFileName)
{
    m_file.setFileName(localFileName);
    QString filePath = remoteFileName.section('/', 0, -2);
    QString fileName = remoteFileName.section('/', -1, -1);
    qDebug() << filePath << fileName;
    QVariantMap map;
    map.insert("filePath", filePath);
    map.insert("fileName", fileName);
    QJsonDocument doc = QJsonDocument::fromVariant(map);

    m_jsonData.clear();
    QDataStream json(&m_jsonData, QIODevice::WriteOnly);
    json.setVersion(QDataStream::Qt_5_4);
    json << qint32(0) << Upload << doc.toJson();
    qint32 size = m_jsonData.size();
    json.device()->seek(0);
    json << size;

    connect(this, &Uploader::connected, this, &Uploader::m_hasConnected);
    connect(this, &Uploader::readyRead, this, &Uploader::m_reply);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));

    this->connectToHost(m_hostName, m_port);
}

void Uploader::cancel()
{
    m_totalBytes = 0;
    m_bytesSend = 0;
    m_bytesToSend = 0;
    m_file.close();
    disconnect(this, &Uploader::connected, this, &Uploader::m_hasConnected);
    disconnect(this, &Uploader::readyRead, this, &Uploader::m_reply);
    disconnect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
    disconnect(this, &Uploader::bytesWritten, this, &Uploader::m_putDataHandler);
    this->disconnectFromHost();
    emit transerCancel();
}

Uploader::~Uploader()
{
//    this->cancel();
}

void Uploader::m_hasConnected()
{
    this->write(m_jsonData);
}

void Uploader::m_reply()
{
    if(this->bytesAvailable() < (qint32)sizeof(qint32))
        return;
    QByteArray replyData = this->readAll();
    QDataStream reply(replyData);
    qint32 ack;
    reply >> ack;
#define UPLOADACK ((qint32)0)
    if(ack == UPLOADACK) {
        if(!m_file.open(QIODevice::ReadOnly)) {
            qDebug() << "client: read file error";
        }
        m_totalBytes = m_bytesToSend = sizeof(qint64) + m_file.size();
        connect(this, &Uploader::bytesWritten, this, &Uploader::m_putDataHandler);
        m_sendSize();
    }
}

void Uploader::m_sendSize()
{
    QByteArray fileSize;
    QDataStream reply(&fileSize, QIODevice::WriteOnly);
    reply.setVersion(QDataStream::Qt_5_4);
    reply << m_totalBytes;
    m_bytesToSend -= this->write(fileSize);
}

void Uploader::m_putDataHandler(qint64 size)
{
    m_bytesSend += size;
    if(m_bytesToSend > 0) {
        QByteArray fileBlock = m_file.read(qMin(m_bytesToSend, PayloadSize));
        m_bytesToSend -= this->write(fileBlock);
    }
    if(m_bytesSend == m_totalBytes) {
        this->disconnectFromHost();
        m_file.close();
        emit uploadFinish();
    }
    emit uploadProgress(m_bytesSend, m_totalBytes);
}

void Uploader::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;

    this->cancel();         //出现错误进行处理
}
