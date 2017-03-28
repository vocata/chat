#include "friendlistwidget.h"
#include "friendwidget.h"
#include "localwidget.h"

#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QToolButton>

#include <QDebug>

FriendListWidget::FriendListWidget(QWidget *parent) : QWidget(parent)
{
    m_localWidget = new LocalWidget(this);
    m_friendScrollArea = new FriendWidget(this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
}

void FriendListWidget::setLoginedInfo(const QMap<QString, QString> &info)
{
    m_localWidget->setLoginedInfo(info);
}

void FriendListWidget::updateFriendList(const QJsonDocument &doc)
{
    QVariantMap map = doc.object().toVariantMap();
    QStringList allAccountList = map.value("allAccountList").toStringList();
    QStringList onlineAccountList = map.value("onlineAccountList").toStringList();
    m_friendScrollArea->updateFriend(allAccountList, onlineAccountList);
}

void FriendListWidget::setPendingReadFlag(const QString &account)
{
    m_friendScrollArea->setPendingReadFlag(account);
}

void FriendListWidget::setState(bool state)
{
    m_localWidget->setState(state);
}

void FriendListWidget::m_setAttribution()
{
    m_localWidget->setFixedHeight(66);
    m_localWidget->setObjectName("localWidget");
    m_friendScrollArea->setObjectName("friendScrollArea");
}

void FriendListWidget::m_setLayout()
{
    QVBoxLayout *box = new QVBoxLayout(this);
    box->addWidget(m_localWidget);
    box->addWidget(m_friendScrollArea);
    box->setSpacing(0);
    box->setMargin(0);
}

void FriendListWidget::m_setSignalSlot()
{
    connect(m_friendScrollArea, &FriendWidget::accountClicked, this, &FriendListWidget::readyChat);
    connect(m_localWidget, &LocalWidget::online, this, &FriendListWidget::online);
    connect(m_localWidget, &LocalWidget::offline, this, &FriendListWidget::offline);
    connect(m_localWidget, &LocalWidget::offline, m_friendScrollArea, &FriendWidget::clearState);
}

void FriendListWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/friendlistwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}


