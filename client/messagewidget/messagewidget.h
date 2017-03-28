#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include "messagewidget/statewidget/statewidget.h"
#include "messagewidget/chatwidget/chatwidget.h"

#include <QMap>
#include <QGroupBox>


class StateWidget;
class HistoryWidget;
class ChatWidget;
class AbstractMessageWidget;

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(QWidget *parent = 0);
    void addReceiveMessage(quint32 type, const QJsonDocument &json);
    void addSendMessage(quint32 type, const QJsonDocument &json);
    void setAccount(const QString &localAccount, const QString &peerAccount);       //设置ChatWidget的account
    void setUser(const QString &user);                                              //设置StateWidget的user
    void setFriendState(bool state);
    void setSendMessageState(quint64 messageId);
    void setState(bool online);
signals:
    void sendMessage(const QByteArray &message);
private:
    StateWidget *m_stateWidget;
    HistoryWidget *m_historyWidget;
    ChatWidget *m_chatWidget;

    QMap<quint64, AbstractMessageWidget *> m_pendingAckMessage;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
    void m_addNormalSendMessage(const QJsonDocument &json);
    void m_addImageSendMessage(const QJsonDocument &json);
    void m_addFileSendMessage(const QJsonDocument &json);
    void m_addNormalReceiveMessage(const QJsonDocument &json);
    void m_addImageReceiveMessage(const QJsonDocument &json);
    void m_addFileReceiveMessage(const QJsonDocument &json);
    void m_sendMessage(quint32 type, const QJsonDocument &json);
};

inline void MessageWidget::setAccount(const QString &localAccount, const QString &peerAccount)
{
    m_chatWidget->setAccount(localAccount, peerAccount);
}
inline void MessageWidget::setUser(const QString &user) { m_stateWidget->setUser(user); }
inline void MessageWidget::setFriendState(bool state) { m_stateWidget->setFriendState(state); }

#endif // MESSAGEWIDGET_H
