#include "widget.h"
#include "message/messagetype.h"
#include "predialog/predialog.h"
#include "messagewidget/messagestackedwidget.h"
#include "friendwidget/friendlistwidget.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>

#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    m_messageStackedWidget = new MessageStackedWidget(this);
    m_friendListWidget = new FriendListWidget(this);
    m_heartBeatTimer = new QTimer(this);
    m_tcpSocket = new QTcpSocket(this);
    m_online = false;

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->setFixedSize(Width, Height);

    qint32 index = QApplication::desktop()->primaryScreen();
    qint32 desktopWidget = QApplication::desktop()->screen(index)->width();
    qint32 desktopHeight = QApplication::desktop()->screen(index)->height();
    this->move((desktopWidget - Width)/2, (desktopHeight - Height)/2);     //窗口居中
    this->setWindowTitle(QStringLiteral("聊天"));
}

void Widget::m_setAttribution()
{
    m_tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    m_messageStackedWidget->setObjectName("messageStackedWidget");
    m_friendListWidget->setFixedWidth(160);
    m_friendListWidget->setObjectName("friendListWidget");

    m_heartBeatTimer->setInterval(15000);
}

void Widget::m_setLayout()
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(m_friendListWidget);
    box->addWidget(m_messageStackedWidget);
    box->setSpacing(0);
    box->setMargin(0);
}

void Widget::m_setSignalSlot()
{
    connect(m_messageStackedWidget, &MessageStackedWidget::newMessageReceive, m_friendListWidget, &FriendListWidget::setPendingReadFlag);
    connect(m_messageStackedWidget, &MessageStackedWidget::sendMessage, this, &Widget::m_sendMessage);
    connect(m_friendListWidget, &FriendListWidget::readyChat, m_messageStackedWidget, &MessageStackedWidget::setCurrentAccountWidget);
    connect(m_friendListWidget, &FriendListWidget::online, this, &Widget::m_startChat);
    connect(m_friendListWidget, &FriendListWidget::offline, this, &Widget::m_stopChat);
    connect(m_heartBeatTimer, &QTimer::timeout, this, &Widget::m_heartBeat);
}

/*
 * summary: 负责处理第一次的登录,，使用ProDialog进行登录处理，并将处理结果返回，
 *          并且连接socket槽
 * param: 无
 * ret: 无
 **/
bool Widget::startChat()
{
    PreDialog preDialog(m_tcpSocket, this);
    if(preDialog.exec() == QDialog::Rejected) {
        return false;
    }
    connect(m_tcpSocket, &QTcpSocket::connected, this, &Widget::m_relogin);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &Widget::m_readReply);
    connect(m_tcpSocket, static_cast<void (QAbstractSocket:: *)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &Widget::m_errorHandler);

    /* 返回登录用户信息 */
    m_loginedInfo = preDialog.getLoginedInfo();
    m_messageStackedWidget->setLoginedInfo(m_loginedInfo);
    m_friendListWidget->setLoginedInfo(m_loginedInfo);

    this->m_getList();
    return true;
}

/*
 * summary: 负责重新登录当前用户，不论是用户自行的下线还是网络出现问题导致的下线，都重新关闭当前连接并进行重新连接，
 *          重新登录的过程中不改变账号和密码.
 * m_stopChat: 离线当前用户
 * m_startChat: 重连当前用户
 * m_relogin: 发送账号密码
 **/
void Widget::m_stopChat()
{
    m_tcpSocket->disconnectFromHost();
    m_heartBeatTimer->stop();
    m_messageStackedWidget->clearFriendState();
    m_messageStackedWidget->setState(false);
    m_online = false;
}

void Widget::m_startChat()
{
    m_tcpSocket->connectToHost("120.77.156.128", 7788);
}

void Widget::m_relogin()
{
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(Login)
        << m_loginedInfo.value("account") << m_loginedInfo.value("passwd");
    quint32 size = request.size();
    out.device()->seek(0);
    out << size;

    m_tcpSocket->write(request);
}

/*
 * summary: 获取好友列表
 * param: 无
 * ret: 无
 **/
void Widget::m_getList()
{
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(GetList);

    quint32 size = request.size();
    out.device()->seek(0);
    out << size;
    m_tcpSocket->write(request);
}

void Widget::m_getRestmessage()
{
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(GetRestMessage);

    quint32 size = request.size();
    out.device()->seek(0);
    out << size;
    m_tcpSocket->write(request);
}

/*
 * summary: 发送心跳包
 * param: 消息Id
 * ret: 无
 **/
void Widget::m_heartBeat()
{
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(HeartBeat);

    quint32 size = request.size();
    out.device()->seek(0);
    out << size;
    m_tcpSocket->write(request);
}

/*
 * summary: 发送确认
 * param: 消息Id
 * ret: 无
 **/
void Widget::m_ack(quint64 messageId)
{
    QByteArray ack;
    QDataStream out(&ack, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(ACK) << messageId;
    out.device()->seek(0);
    quint32 size = ack.size();
    out << size;
    m_tcpSocket->write(ack);
}

/*
 * summary: 发送信息
 * param: 无
 * ret: 无
 **/
void Widget::m_sendMessage(QByteArray message)
{
    m_tcpSocket->write(message);
}

/*
 * summary: 根据数据类型分发数据
 * param: 无
 * ret: 无
 **/
void Widget::m_readReply()
{
    static QByteArray fullData;
    static quint32 bytesReceived = 0, totalBytes = 0;
    quint32 type;

    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);

    if(bytesReceived < 2 * sizeof(quint32)) {
        if(m_tcpSocket->bytesAvailable() >= (qint64)(2 * sizeof(quint32))) {
            in >> totalBytes >> type;
            bytesReceived += 2 * sizeof(quint32);
        } else {
            return;
        }
    }

    if(bytesReceived < totalBytes) {
        QByteArray temp;
        temp = m_tcpSocket->read(totalBytes - bytesReceived);
        fullData.append(temp);
        bytesReceived += temp.size();
    }

    if(bytesReceived == totalBytes) {
        switch(type) {
        case Login: {
            this->m_reloginHandler(fullData);
            break;
        }
        case GetList: {
            this->m_getListHandler(fullData);
            break;
        }
        case Message: case Image: case File: {
            this->m_messageHandler(fullData, type);
            break;
        }
        case GetRestMessage: {
            this->m_restMessageHandler(fullData);
            break;
        }
        case ACK:{
            this->m_ackHandler(fullData);
            break;
        }
        default: break;
        }

        fullData.clear();
        bytesReceived = 0;
        totalBytes = 0;
    }
}

/*
 * summary: 重新登录数据处理
 * param: 无
 * ret: 无
 **/
void Widget::m_reloginHandler(QByteArray data)
{
    QDataStream reply(&data, QIODevice::ReadOnly);
    quint32 result;
    QString user;
    reply >> result >> user;

    if(result == LoginSuccess) {
        m_loginedInfo["user"] = user;
        m_messageStackedWidget->setLoginedInfo(m_loginedInfo);
        m_friendListWidget->setLoginedInfo(m_loginedInfo);

        this->m_getList();
    } else {
#ifndef EXIT_RESTART
#define EXIT_RESTART 774
#endif
        QMessageBox::information(this, QStringLiteral("登录失败"), QStringLiteral("密码已修改\n请重新登录!"));
        QApplication::exit(EXIT_RESTART);
    }
}

/*
 * summary: 对获得的好友列表进行处理
 * param: 无
 * ret: 无
 **/
void Widget::m_getListHandler(QByteArray data)
{
    QDataStream reply(&data, QIODevice::ReadOnly);
    reply.setVersion(QDataStream::Qt_5_4);

    QByteArray json;
    reply >> json;
    QJsonDocument doc = QJsonDocument::fromJson(json);
    m_messageStackedWidget->updateFriendList(doc);
    m_friendListWidget->updateFriendList(doc);              //更新好友列表
    m_messageStackedWidget->setState(true);
    m_friendListWidget->setState(true);

    if(!m_online) {
        this->m_getRestmessage();                               //获取好友列表后获取未读信息
        m_online = true;
    }

    m_heartBeatTimer->start();                              //打开心跳
    this->show();         //这里调用show()可以保证一定会显示窗口
}

/*
 * summary: 接收消息处理
 * param: 无
 * ret: 无
 **/
void Widget::m_messageHandler(QByteArray data, quint32 type)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_4);
    QByteArray json;
    in >> json;

    QJsonDocument message = QJsonDocument::fromJson(json);
    m_messageStackedWidget->addReceiveMessage(type, message);

    quint64 messageId = message.object().toVariantMap().value("messageId").toULongLong();
    this->m_ack(messageId);
}

/*
 * summary: 剩余消息接收处理
 * param: 无
 * ret: 无
 **/
void Widget::m_restMessageHandler(QByteArray data)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_4);
    QList<QByteArray> dataList;
    in >> dataList;
    for(auto &data: dataList) {
        QDataStream in(&data, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_4);
        quint32 type;
        QByteArray json;
        in >> type >> json;

        QJsonDocument message = QJsonDocument::fromJson(json);
        m_messageStackedWidget->addReceiveMessage(type, message);
    }
    this->m_ack(0);
}

/*
 * summary: 接收ack处理
 * param: 无
 * ret: 无
 **/
void Widget::m_ackHandler(QByteArray data)
{
    QDataStream in(&data, QIODevice::ReadOnly);
    QString peerAccount;
    quint64 messageId;
    in >> peerAccount >> messageId;
    m_messageStackedWidget->setSendMessageState(peerAccount, messageId);
}

/*
 * summary: 网络出现错误时进行相应的处理
 * param: 无
 * ret: 无
 **/void Widget::m_errorHandler(QAbstractSocket::SocketError error)
{
    switch(error) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::SocketTimeoutError:
        break;
    default:
        break;
    }
    m_messageStackedWidget->setState(false);
    m_friendListWidget->setState(false);
    qDebug() << error;
}
