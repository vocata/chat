#include "friendwidget.h"
#include "friendwidget/friendButton/friendbutton.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QButtonGroup>

#include <QDebug>

FriendWidget::FriendWidget(QWidget *parent) : QScrollArea(parent)
{
    m_contentWidget = new QWidget(this);
    m_buttonGroup = new QButtonGroup(this);
    m_space = new QWidget(this);
    m_buttonBox = new QVBoxLayout();

    m_contentWidget->setObjectName("contentWidget");
    m_space->setFixedHeight(0);

    m_buttonBox->setSpacing(0);
    m_buttonBox->setMargin(0);

    QVBoxLayout *box = new QVBoxLayout(m_contentWidget);
    box->addLayout(m_buttonBox);
    box->addWidget(m_space, 0, Qt::AlignTop);
    box->setSpacing(0);
    box->setMargin(0);

    this->setWidget(m_contentWidget);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setObjectName("historyScrollBar");
}

void FriendWidget::updateFriend(const QStringList &allAccountList, const QStringList &onlineAccountList)
{
    for(const QString &fri: allAccountList) {
        QString account = fri.section(':', 0, -2);
        QString user = fri.section(':', -1, -1);        //info[0] = "account", info[1] = "user"

        if(m_friends.contains(account)) {
            FriendButton *oldFriend = m_friends.value(account);

            if(onlineAccountList.contains(account)) {
                oldFriend->setState(true);
                m_buttonBox->insertWidget(0, oldFriend);
            } else {
                oldFriend->setState(false);
                m_buttonBox->insertWidget(-1, oldFriend);
            }
        } else {
            FriendButton *newFriend = new FriendButton(this);
            newFriend->setAccountAndUser(account, user);
            m_buttonGroup->addButton(newFriend);
            connect(newFriend, &FriendButton::doubleClicked, this, &FriendWidget::accountClicked);

            if(onlineAccountList.contains(account)) {
                newFriend->setState(true);
                m_buttonBox->insertWidget(0, newFriend);
            } else {
                newFriend->setState(false);
                m_buttonBox->insertWidget(-1, newFriend);
            }

            m_friends.insert(account, newFriend);
        }
    }
}

void FriendWidget::clearState()
{
    for(auto widget: m_friends) {
        widget->setState(false);
    }
}

void FriendWidget::setPendingReadFlag(const QString &account)
{
    m_friends.value(account)->setPendingReadFlag();
}

