#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QTcpSocket>

class QTimer;

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocket(QTcpSocket *socket, QObject *parent = 0);
    QTcpSocket *getTcpSocket();
    void setOnline(bool isOnline);
    bool getOnline();
    void close();
    void write(QByteArray data);
    void flush();
    ~TcpSocket();

signals:
    void readyRead(TcpSocket *connection, QByteArray data);
    void disconnected(TcpSocket *connection);
    void connectBroken(TcpSocket *connection);

private:
    QTcpSocket *m_tcpSocket;
    bool m_isOnline;
    QTimer *m_timer;

    QByteArray m_fullData;
    quint32 m_bytesReceived;
    quint32 m_totalBytes;

private slots:
    void m_judgeOnline();
    void m_sendReadyRead();
    void m_sendDisconnected();
};

inline QTcpSocket *TcpSocket::getTcpSocket() { return m_tcpSocket; }
inline bool TcpSocket::getOnline() { return m_isOnline; }
inline void TcpSocket::close() { m_tcpSocket->close(); }
inline void TcpSocket::write(QByteArray data) { m_tcpSocket->write(data); }
inline void TcpSocket::flush() { m_tcpSocket->flush(); }

#endif // TCPSOCKET_H
