#include "filehandler.h"

#include <QDataStream>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>

#include <QDebug>

FileHandler::FileHandler(QObject *parent) : QTcpSocket(parent)
{
    m_preDataTotalBytes = 0;
    m_preDataBytesReceived = 0;
    m_fileDataTotalBytes = 0;
    m_fileDataBytesReceived = 0;
    m_fileDataBytesSend = 0;
    m_fileDataBytesToSend = 0;
}

void FileHandler::start(qintptr connfd)
{
    qDebug() << connfd;
    this->setSocketDescriptor(connfd);
    connect(this, &FileHandler::readyRead, this, &FileHandler::m_selectTransferType);
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
}

void FileHandler::m_selectTransferType()
{
    disconnect(this, &FileHandler::readyRead, this, &FileHandler::m_selectTransferType);
    if(m_preDataBytesReceived < (qint32)sizeof(qint32)) {
        if(this->bytesAvailable() < (qint64)sizeof(qint32))
            return;

        QDataStream in(this);
        in.setVersion(QDataStream::Qt_5_4);
        in >> m_preDataTotalBytes;
        m_preDataBytesReceived += sizeof(qint32);
    }

    if(m_preDataBytesReceived < m_preDataTotalBytes) {
        QByteArray temp;
        temp = this->read(m_preDataTotalBytes - m_preDataBytesReceived);
        m_preData.append(temp);
        m_preDataBytesReceived += temp.size();
    }

    if(m_preDataBytesReceived == m_preDataTotalBytes) {
        QDataStream request(m_preData);
        request.setVersion(QDataStream::Qt_5_4);

        qint32 type;
        QByteArray json;
        request >> type >> json;
        switch(type) {
        case Download: m_putData(json); break;
        case Upload: m_getData(json); break;
        }
    }
}

void FileHandler::m_putData(QByteArray json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QVariantMap map = doc.object().toVariantMap();
    QString filePath = map.value("filePath").toString();
    QString fileName = map.value("fileName").toString();
    QString fileFullName = QDir::currentPath() + "/" + filePath + "/" + fileName;
    qDebug() << QString("file path: %1").arg(fileFullName);
    m_localFile.setFileName(fileFullName);
    if(!m_localFile.exists()) {
        qDebug() << "server: file not exists";
        return;
    }
    if(!m_localFile.open(QIODevice::ReadOnly)) {
        qDebug() << "server: open file error!";
        return;
    }
    m_fileDataTotalBytes = m_fileDataBytesToSend = sizeof(qint64) + m_localFile.size();

    connect(this, &FileHandler::bytesWritten, this, &FileHandler::m_putDataHandler);
    connect(this, &FileHandler::disconnected, this, &FileHandler::m_downloadCloseHandler);
    this->m_putDataAck();
}

void FileHandler::m_putDataAck()
{
    QByteArray fileSize;
    QDataStream reply(&fileSize, QIODevice::WriteOnly);
    reply.setVersion(QDataStream::Qt_5_4);
    reply << m_fileDataTotalBytes;
    m_fileDataBytesToSend -= this->write(fileSize);
}

void FileHandler::m_putDataHandler(qint64 size)
{
    m_fileDataBytesSend += size;
    if(m_fileDataBytesToSend > 0) {
        QByteArray fileBlock = m_localFile.read(qMin(m_fileDataBytesToSend, PayloadSize));
        m_fileDataBytesToSend -= this->write(fileBlock);
    }
    if(m_fileDataBytesSend == m_fileDataTotalBytes) {
        qDebug() << "download finish";
    }
}

void FileHandler::m_downloadCloseHandler()
{
    m_localFile.close();
    qDebug() << "downloader quit";
    QCoreApplication::quit();
}



void FileHandler::m_getData(QByteArray json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json);
    QVariantMap map = doc.object().toVariantMap();
    QString filePath = map.value("filePath").toString();
    QString fileName = map.value("fileName").toString();
    QString fileFullName = QDir::currentPath() + "/" + filePath + "/" + fileName;
    qDebug() << QString("file path: %1").arg(fileFullName);
    QDir().mkpath(QDir::currentPath() + "/" + filePath);
    m_localFile.setFileName(fileFullName);
    if(!m_localFile.open(QIODevice::WriteOnly)) {
        qDebug() << "server: open file error!";
        return;
    }

    connect(this, &FileHandler::readyRead, this, &FileHandler::m_getDataHandler);
    connect(this, &FileHandler::disconnected, this, &FileHandler::m_uploadCloseHandler);
    this->m_getDataAck();
}

void FileHandler::m_getDataAck()
{
    QByteArray replyData;
    QDataStream reply(&replyData, QIODevice::WriteOnly);
    reply.setVersion(QDataStream::Qt_5_4);
#define UPLOADACK ((qint32)0)
    reply << UPLOADACK;
    this->write(replyData);
    qDebug() << "getDataAck";
}

void FileHandler::m_getDataHandler()
{
    if(m_fileDataBytesReceived < (qint64)sizeof(qint64)) {
        if(this->bytesAvailable() < (qint64)sizeof(qint64))
            return;
        QDataStream in(this);
        in.setVersion(QDataStream::Qt_5_4);
        in >> m_fileDataTotalBytes;
        m_fileDataBytesReceived += sizeof(qint64);
    }
    if(m_fileDataBytesReceived < m_fileDataTotalBytes) {
        QByteArray fileBlock;
        fileBlock = this->read(m_fileDataTotalBytes - m_fileDataBytesReceived);
        m_localFile.write(fileBlock);
        m_fileDataBytesReceived += fileBlock.size();
    }
    if(m_fileDataBytesReceived == m_fileDataTotalBytes)
        qDebug() << "upload finish";
}

void FileHandler::m_uploadCloseHandler()
{
    m_localFile.close();

    if(m_fileDataBytesReceived != m_fileDataTotalBytes)
        m_localFile.remove();
    qDebug() << "upload quit";
    QCoreApplication::quit();
}

void FileHandler::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;
}

