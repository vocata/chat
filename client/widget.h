#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QAbstractSocket>

class QTcpSocket;
class MessageStackedWidget;
class FriendListWidget;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
public:
    bool startChat();
private:
    MessageStackedWidget *m_messageStackedWidget;
    FriendListWidget *m_friendListWidget;
    QTimer *m_heartBeatTimer;
    QTcpSocket *m_tcpSocket;
    QMap<QString, QString> m_loginedInfo;
    bool m_online;
    const qint32 Width = 800;
    const qint32 Height = 550;

    void m_reloginHandler(QByteArray data);
    void m_getListHandler(QByteArray data);
    void m_messageHandler(QByteArray data, quint32 type);
    void m_restMessageHandler(QByteArray data);
    void m_ackHandler(QByteArray data);
    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
private slots:
    void m_stopChat();
    void m_startChat();
    void m_relogin();
    void m_getList();
    void m_getRestmessage();
    void m_heartBeat();
    void m_ack(quint64 messageId);
    void m_sendMessage(QByteArray message);

    void m_readReply();
    void m_errorHandler(QAbstractSocket::SocketError error);
};
#endif // WIDGET_H
