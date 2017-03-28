#include "server.h"
#include "sql/connection.h"
#include "tcp/tcpsocket.h"

#include <QDebug>

Server::Server(QObject *parent) : QObject(parent)
{
    m_loginServer = new QTcpServer(this);

    m_loginServer->setMaxPendingConnections(1024);
    connect(m_loginServer, &QTcpServer::newConnection, this, &Server::m_tcpAcceptConnection);
    m_loginServer->listen(QHostAddress::Any, CHAT_SERVER_PORT);

    createConnection();
}

void Server::loginHandler(TcpSocket *connection, const QString &account, const QString &passwd)
{
    QByteArray loginResult;
    QDataStream out(&loginResult, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(Login);

    QSqlQuery query;
    query.exec(QString("select passwd, user from user_info where account = %1").arg(account));

    if(query.next()) {
        QString queryPasswd = query.value(0).toString();
        if(queryPasswd == passwd) {          
            connection->setOnline(true);
            m_tcpConnectionMap.insert(connection, account);
            m_AccountMap.insert(account, connection);

            QString user = query.value(1).toString();
            out << static_cast<quint32>(LoginSuccess) << user;       // success send user name.
            qDebug() << "login success";

            for(auto tcpConnection: m_AccountMap) {
                if(tcpConnection == connection) continue;
                this->getListHandler(tcpConnection);
            }
        } else {
            out << static_cast<quint32>(PasswdError);
            qDebug() << "incorrect passwd";
        }
    } else {
        out << static_cast<quint32>(NoAccount);
        qDebug() << "no account";
    }
    quint32 size = loginResult.size();
    out.device()->seek(0);
    out << size;
    connection->write(loginResult);
    connection->flush();
}

void Server::registerHandler(TcpSocket *connection, const QString &user, const QString &account, const QString &passwd)
{
    QByteArray registerResult;
    QDataStream out(&registerResult, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(Register);

    if(account.size() == AccountSize
            && passwd.size() >= PasswdMinSize && passwd.size() <= PasswdMaxSize
            && user.size() >= UserMinSize && user.size() <= UserMaxSize) {

        QSqlQuery query;
        bool result = query.exec(QString("insert into user_info values('%1', '%2', '%3')").arg(account, passwd, user));

        if(!result) {
            out << static_cast<quint32>(RegisterFailure);
            qDebug() << "register failuer";
        } else {
            out << static_cast<quint32>(RegisterSuccess);
            qDebug() << "register success";
        }
    } else {
        out << static_cast<quint32>(RegisterFailure);
        qDebug() << "register error";
    }
    quint32 size = registerResult.size();
    out.device()->seek(0);
    out << size;
    connection->write(registerResult);
    connection->flush();
}

void Server::getListHandler(TcpSocket *connection)
{
    if(!m_tcpConnectionMap.contains(connection))
        return;

    QByteArray getListResult;
    QDataStream out(&getListResult, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(GetList);

    QSqlQuery query;
    query.exec(QString("select account, user from user_info where account != '%1'").arg(m_tcpConnectionMap.value(connection)));

    QVariantMap map;
    QStringList allAccountList;
    while(query.next()) {
        QString account = query.value(0).toString();
        QString user = query.value(1).toString();
        allAccountList.push_back(account + ":" + user);
    }
    map.insert("allAccountList", allAccountList);

    QStringList onlineAccountList;
    for(auto account: m_tcpConnectionMap) {
        onlineAccountList.push_back(account);
    }
    map.insert("onlineAccountList", onlineAccountList);

    QJsonDocument doc = QJsonDocument::fromVariant(map);
    out << doc.toJson();

    quint32 size = getListResult.size();
    out.device()->seek(0);
    out << size;
    connection->write(getListResult);
    connection->flush();
}

void Server::infoForwardHandler(TcpSocket *connection, QByteArray json, quint32 type)
{
    if(!m_tcpConnectionMap.contains(connection))
        return;

    QVariantMap map = QJsonDocument::fromJson(json).object().toVariantMap();
    QString peerAccount = map.value("peerAccount").toString();
    quint64 messageId = map.value("messageId").toULongLong();

    //回复消息已收到
    QByteArray replyMessage;
    QDataStream reply(&replyMessage, QIODevice::WriteOnly);
    reply.setVersion(QDataStream::Qt_5_4);
    reply << static_cast<quint32>(0) << ACK << peerAccount << messageId;

    quint32 size = replyMessage.size();
    reply.device()->seek(0);
    reply << size;
    connection->write(replyMessage);
    connection->flush();

    //插入到数据库，直到收到回复
    QSqlQuery query;
    query.prepare("insert into user_message values(:peerAccount, :type, :messageId, :message)");
    query.bindValue(":type", type);
    query.bindValue(":peerAccount", peerAccount);
    query.bindValue(":messageId", QString::number(messageId));
    query.bindValue(":message", QString(json).simplified());
    query.exec();

    //转发信息
    if(m_AccountMap.contains(peerAccount)) {
        QByteArray forwardMessage;
        QDataStream forward(&forwardMessage, QIODevice::WriteOnly);
        forward.setVersion(QDataStream::Qt_5_4);
        forward << static_cast<quint32>(0) << type << json;

        size = forwardMessage.size();
        forward.device()->seek(0);
        forward << size;
        TcpSocket *peerConnection = m_AccountMap.value(peerAccount);
        peerConnection->write(forwardMessage);
        peerConnection->flush();
    }
}

void Server::getRestMessageHandler(TcpSocket *connection)
{
    if(!m_tcpConnectionMap.contains(connection))
        return;

    QString peerAccount = m_tcpConnectionMap.value(connection);
    QSqlQuery query;
    query.exec(QString("select type, message from user_message where peerAccount = '%1'").arg(peerAccount));

    quint32 type;
    QByteArray json;
    QList<QByteArray> dataList;
    while(query.next()) {
        type = query.value(0).toUInt();
        json = query.value(1).toString().toUtf8();

        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);
        out << type << json;
        dataList.push_back(data);
    }
    if(dataList.isEmpty())
        return;

    QByteArray restMessage;
    QDataStream out(&restMessage, QIODevice::WriteOnly);
    out << static_cast<quint32>(0) << static_cast<quint32>(GetRestMessage) << dataList;

    out.device()->seek(0);
    quint32 size = restMessage.size();
    out << size;
    connection->write(restMessage);
    connection->flush();
}

void Server::ackHandler(TcpSocket *connection, quint64 messageId)
{
    if(!m_tcpConnectionMap.contains(connection))
        return;

    QString peerAccount = m_tcpConnectionMap.value(connection);
    QSqlQuery query;
    if(messageId == 0) {
        query.exec(QString("delete from user_message where peerAccount = '%1'")
                   .arg(peerAccount));

    } else {
        query.exec(QString("delete from user_message where peerAccount = '%1' and messageId = %2")
                   .arg(peerAccount, QString::number(messageId)));
    }
}

Server::~Server()
{
    closeConnection();
}

void Server::m_tcpAcceptConnection()
{
    while(m_loginServer->hasPendingConnections()) {
        QTcpSocket *socket = m_loginServer->nextPendingConnection();
        socket->setSocketOption(QAbstractSocket::LowDelayOption, 0);

        TcpSocket *connection = new TcpSocket(socket, this);

        connect(connection, &TcpSocket::readyRead, this, &Server::m_tcpMessageHandler);
        connect(connection, &TcpSocket::connectBroken, this, &Server::m_tcpBrokenHandler);
        connect(connection, &TcpSocket::disconnected, this, &Server::m_tcpBrokenHandler);
    }
}

void Server::m_tcpMessageHandler(TcpSocket *connection, QByteArray data)
{
    quint32 type;
    QDataStream request(data);
    request.setVersion(QDataStream::Qt_5_4);

    request >> type;

    switch(type) {
    case Login: {
        QString account, passwd;
        request >> account >> passwd;
        this->loginHandler(connection, account, passwd);
        break;
    }
    case Register: {
        QString user, account, passwd;
        request >> user >> account >> passwd;
        this->registerHandler(connection, user, account, passwd);
        break;
    }
    case GetList: {
        this->getListHandler(connection);
        break;
    }
    case Message: case Image: case File: {
        QByteArray json;
        request >> json;
        this->infoForwardHandler(connection, json, type);
        break;
    }
    case GetRestMessage: {
        this->getRestMessageHandler(connection);
        break;
    }
    case ACK: {
        quint64 messageId;
        request >> messageId;
        this->ackHandler(connection, messageId);
        break;
    }

    default: break;
    }
}

void Server::m_tcpBrokenHandler(TcpSocket *connection)
{
    m_AccountMap.remove(m_tcpConnectionMap.value(connection));
    m_tcpConnectionMap.remove(connection);
    for(auto tcpConnection: m_AccountMap) {
        if(tcpConnection == connection) continue;
        this->getListHandler(tcpConnection);
    }

    delete connection;
    qDebug() << "connection disconnection";
}
