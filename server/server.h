#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtNetwork>
#include <QtSql>

#define CHAT_SERVER_PORT 7788

enum TcpMsgType: quint32 { Login = 0, Logout, Register, GetList, Message, Image, File, ACK, GetRestMessage, HeartBeat };
enum LoginStatus: quint32 { LoginSuccess = 20, PasswdError , NoAccount };
enum RegisterStatus: quint32 { RegisterSuccess = 30, RegisterFailure };

class TcpSocket;
class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = 0);
    ~Server();
private:
    QTcpServer *m_loginServer;
    QUdpSocket *m_udpServer;
    QMap<TcpSocket *, QString>m_tcpConnectionMap;
    QMap<QString, TcpSocket *>m_AccountMap;

    void loginHandler(TcpSocket *connection, const QString &account, const QString &passwd);
    void registerHandler(TcpSocket *connection, const QString &user, const QString &account, const QString &passwd);
    void getListHandler(TcpSocket *connection);
    void infoForwardHandler(TcpSocket *connection, QByteArray json, quint32 type);
    void getRestMessageHandler(TcpSocket *connection);
    void ackHandler(TcpSocket *connection, quint64 messageId);
private slots:
    void m_tcpAcceptConnection();
    void m_tcpMessageHandler(TcpSocket *connection, QByteArray data);
    void m_tcpBrokenHandler(TcpSocket *connection);
};

const qint32 UserMinSize = 2;
const qint32 UserMaxSize = 20;
const qint32 AccountSize = 11;
const qint32 PasswdMaxSize = 16;
const qint32 PasswdMinSize = 6;

#endif // SERVER_H
