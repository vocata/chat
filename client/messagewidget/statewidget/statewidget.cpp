#include "statewidget.h"

#include <QVBoxLayout>
#include <QLabel>

StateWidget::StateWidget(QWidget *parent) : QGroupBox(parent)
{
    m_userLabel = new QLabel(this);
    m_stateLabel = new QLabel(this);
    m_stateIcon = new QLabel(this);

    this->m_setAttribution();
    this->m_setLayout();
}

void StateWidget::setFriendState(bool state)
{
    QPixmap pix;
    if(state) {
        m_stateLabel->setText(QStringLiteral("在线"));
        pix.load(":/statewidget/online");
        m_stateIcon->setPixmap(pix);
    } else {
        m_stateLabel->setText(QStringLiteral("离线"));
        pix.load(":/statewidget/offline");
        m_stateIcon->setPixmap(pix);
    }
}

void StateWidget::m_setAttribution()
{
    m_userLabel->setFixedHeight(20);
    m_userLabel->setObjectName("userLabel");
    m_stateLabel->setObjectName("stateLabel");

    m_stateIcon->setFixedSize(5, 5);
    m_stateIcon->setScaledContents(true);
    m_stateIcon->setObjectName("stateIcon");
}

void StateWidget::m_setLayout()
{
    QHBoxLayout *userIconBox = new QHBoxLayout();
    userIconBox->addWidget(m_stateIcon);
    userIconBox->addWidget(m_userLabel);
    userIconBox->setSpacing(10);

    QVBoxLayout *box = new QVBoxLayout(this);
    box->addLayout(userIconBox);
    box->addWidget(m_stateLabel);
    box->setAlignment(userIconBox, Qt::AlignRight);
    box->setAlignment(m_stateLabel, Qt::AlignRight);
    box->setContentsMargins(-1, -1, 20, -1);
}

