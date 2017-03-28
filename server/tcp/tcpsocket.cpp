#include "tcpsocket.h"
#include <QDataStream>
#include <QTimer>

TcpSocket::TcpSocket(QTcpSocket *socket, QObject *parent) : QObject(parent)
{
    m_tcpSocket = socket;
    m_timer = new QTimer(this);
    m_isOnline = false;
    m_bytesReceived = 0;
    m_totalBytes = 0;

    m_timer->setInterval(450000);       //7.5m
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &TcpSocket::m_sendReadyRead);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &TcpSocket::m_sendDisconnected);
}

void TcpSocket::setOnline(bool isOnline)
{
    m_isOnline = isOnline;
    connect(m_timer, &QTimer::timeout, this, &TcpSocket::m_judgeOnline);
    m_timer->start();
}

TcpSocket::~TcpSocket()
{
    m_tcpSocket->deleteLater();
}

void TcpSocket::m_judgeOnline()
{
    if(m_isOnline)
        m_isOnline = false;
    else
        emit connectBroken(this);   //Abnormal disconnectd
}

void TcpSocket::m_sendReadyRead()
{
    m_isOnline = true;      //Data arrival indicates connection is establishing

    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);

    if(m_bytesReceived < sizeof(quint32)) {
        if(m_tcpSocket->bytesAvailable() >= sizeof(quint32)) {
            in >> m_totalBytes;
            m_bytesReceived += sizeof(quint32);
        } else {
            return;
        }
    }

    if(m_bytesReceived < m_totalBytes) {
        QByteArray temp;
        temp = m_tcpSocket->read(m_totalBytes - m_bytesReceived);
        m_fullData.append(temp);
        m_bytesReceived += temp.size();
    }

    if(m_bytesReceived == m_totalBytes) {
        emit readyRead(this, m_fullData);

        m_fullData.clear();
        m_bytesReceived = 0;
        m_totalBytes = 0;
    }
}

void TcpSocket::m_sendDisconnected()
{
    emit disconnected(this);       //normal disconnected;
}


