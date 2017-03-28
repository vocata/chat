#include "friendbutton.h"

#include <QCloseEvent>
#include <QLabel>
#include <QVBoxLayout>

#include <QDebug>

FriendButton::FriendButton(QWidget *parent) : QPushButton(parent)
{
    m_stateIcon = new QLabel(this);
    m_userLabel = new QLabel(this);
    m_stateLabel = new QLabel(this);
    m_pendingReadFlag = new QLabel(this);

    this->setCheckable(true);
    this->m_setAttribution();
    this->m_setLayout();
    this->m_setStyleSheet();
    this->setFixedHeight(65);
    this->setCursor(Qt::PointingHandCursor);
}

void FriendButton::setInfo(const QString &account, const QString &user, bool online)
{
    m_account = account;
    m_user = user;
    m_online = online;
}

void FriendButton::setAccountAndUser(const QString &account, const QString &user)
{
    m_account = account;
    m_user = user;
    QFontMetrics elideFont(m_userLabel->font());
    m_userLabel->setText(elideFont.elidedText(m_user, Qt::ElideRight, m_userLabel->width()));

    this->setToolTip(QString(QStringLiteral("用户: %1\n账号: %2")).arg(user, account));
}

void FriendButton::setState(bool online)
{
    m_online = online;

    if(m_online) {
        m_stateIcon->setPixmap(QPixmap(":/statewidget/online"));
        m_stateLabel->setText(QStringLiteral("在线"));
    } else {
        m_stateIcon->setPixmap(QPixmap(":/statewidget/offline"));
        m_stateLabel->setText(QStringLiteral("离线"));
    }
}

void FriendButton::setPendingReadFlag()
{
    m_pendingReadFlag->setPixmap(QPixmap(":/statewidget/newmessage"));
}

void FriendButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        QWidget::mousePressEvent(e);
        return;
    }
    QPushButton::mousePressEvent(e);
}

void FriendButton::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        this->setChecked(true);
        emit doubleClicked(this->getAccount());
        m_pendingReadFlag->clear();
        e->accept();
        return;
    }

    QPushButton::mouseDoubleClickEvent(e);
}

void FriendButton::m_setAttribution()
{
    m_stateIcon->setFixedSize(25, 25);
    m_stateIcon->setScaledContents(true);
    m_stateIcon->setObjectName("stateIcon");

    QFont userLabelSize;
    userLabelSize.setPointSize(12);
    m_userLabel->setFont(userLabelSize);
    m_userLabel->setMaximumWidth(80);
    m_userLabel->setObjectName("userLabel");

    QFont stateLabelSize;
    stateLabelSize.setPointSize(10);
    m_stateLabel->setFont(stateLabelSize);
    m_stateLabel->setObjectName("stateLabel");

    m_pendingReadFlag->setFixedSize(15, 15);
    m_pendingReadFlag->setScaledContents(true);
    m_pendingReadFlag->setObjectName("pendingReadFlag");
}

void FriendButton::m_setLayout()
{
    QVBoxLayout *labelBox = new QVBoxLayout();
    labelBox->addWidget(m_userLabel);
    labelBox->addWidget(m_stateLabel);

    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(m_stateIcon);
    box->addLayout(labelBox);
    box->addWidget(m_pendingReadFlag);
    box->setAlignment(m_pendingReadFlag, Qt::AlignRight | Qt::AlignBottom);
    box->setSpacing(10);
    box->setMargin(10);
}

void FriendButton::m_setStyleSheet()
{
    QFile qssFile(":/qss/friendbutton");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

