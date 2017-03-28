#include "messagestackedwidget.h"
#include "messagewidget.h"
#include "messagewidget/startwidget/startwidget.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QMap>
#include <QSound>
#include <QVariant>

#include <QDebug>

MessageStackedWidget::MessageStackedWidget(QWidget *parent) : QStackedWidget(parent)
{
    m_startWidget = new StartWidget(this);
    m_tips = new QSound(":/sound/new", this);
    m_startWidget->setObjectName("startWidget");
    this->addWidget(m_startWidget);
}

void MessageStackedWidget::addReceiveMessage(quint32 type, const QJsonDocument &json)
{
    QVariantMap map = json.object().toVariantMap();
    QString peerAccount = map.value("localAccount").toString();     //localAccount代表发送端，也就是相对于我端的peerAccoun

    m_friends.value(peerAccount)->addReceiveMessage(type, json);
    if(m_currentMessageWidgetAccount != peerAccount) {
        emit newMessageReceive(peerAccount);
        m_tips->play();
    }
}

void MessageStackedWidget::setCurrentAccountWidget(const QString &account)
{
    if(m_friends.contains(account)) {
        MessageWidget *oldWidget = m_friends.value(account);
        this->setCurrentWidget(oldWidget);
        m_currentMessageWidgetAccount = account;
    }
}

void MessageStackedWidget::updateFriendList(const QJsonDocument &doc)
{
    QVariantMap map = doc.object().toVariantMap();
    QStringList allAccountList = map.value("allAccountList").toStringList();
    QStringList onlineAccountList = map.value("onlineAccountList").toStringList();

    for(const QString &fri: allAccountList) {
        QString localAccount = m_loginInfo.value("account");
        QStringList info = fri.split(":");          //info[0] = "account", info[1] = "user"
        QString peerAccount = info.at(0), user = info.at(1);

        if(m_friends.contains(peerAccount)) {
            MessageWidget *oldFriend = m_friends.value(peerAccount);

            if(onlineAccountList.contains(peerAccount)) {
                oldFriend->setFriendState(true);
            } else {
                oldFriend->setFriendState(false);
            }
        } else {
            MessageWidget *newFriend = new MessageWidget(this);
            connect(newFriend, &MessageWidget::sendMessage, this, &MessageStackedWidget::sendMessage);
            newFriend->setAccount(localAccount, peerAccount);           //新建的聊天窗口属于那两个客户的连接
            newFriend->setUser(user);
            this->addWidget(newFriend);

            if(onlineAccountList.contains(peerAccount)) {
                newFriend->setFriendState(true);
            } else {
                newFriend->setFriendState(false);
            }

            m_friends.insert(peerAccount, newFriend);
        }
    }
}

void MessageStackedWidget::clearFriendState()
{
    for(auto widget: m_friends) {
        widget->setFriendState(false);
    }
}

void MessageStackedWidget::setSendMessageState(const QString &peerAccount, quint64 messageId)
{
    MessageWidget *messageWidget = m_friends.value(peerAccount);
    messageWidget->setSendMessageState(messageId);
}

void MessageStackedWidget::setState(bool online)
{
    for(auto msgWidget: m_friends) {
        msgWidget->setState(online);
    }
}



