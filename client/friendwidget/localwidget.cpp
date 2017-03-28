#include "localwidget.h"

#include <QApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>

#include <QDebug>

LocalWidget::LocalWidget(QWidget *parent) : QGroupBox(parent)
{
    m_userLabel = new QLabel(this);
    m_stateIcon = new QPushButton(this);
    m_stateMenu = new QMenu(this);
    m_onlineAction = new QAction(QIcon(":/localwidget/online"), QStringLiteral("在线"), this);
    m_offlineAction = new QAction(QIcon(":/localwidget/offline"), QStringLiteral("离线"), this);
    m_restartAction = new QAction(QStringLiteral("重新登录"), this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void LocalWidget::setLoginedInfo(const QMap<QString, QString> &info)
{
    QFontMetrics elideFont(m_userLabel->font());
    m_userLabel->setText(elideFont.elidedText(info.value("user"), Qt::ElideRight, m_userLabel->width()));
    m_userLabel->setToolTip(QString("用户: %1\n账号: %2").arg(info.value("user"), info.value("account")));
}

/*
 * summary: 通过登录结果或网络出现的错误设置相应的登录状态，属于被动设置网络状态。
 *          另外还有m_setState用于手动设置当前登录状态。详细见m_setState
 * param: state 登录的结果
 * ret: 无
 * note: 不论是手动还是被动，当离线时都会发送offline信号，用于改变好友的在线状态
 **/
void LocalWidget::setState(bool state)
{
    m_online = state;

    if(m_online) {
        m_stateIcon->setIcon(QIcon(":/localwidget/online"));
    } else {
        m_stateIcon->setIcon(QIcon(":/localwidget/offline"));
        emit offline();
    }
}

void LocalWidget::m_setAttribution()
{
    QFont size;
    size.setPointSize(16);
    m_userLabel->setFont(size);
    m_userLabel->setCursor(Qt::PointingHandCursor);
    m_userLabel->setMaximumWidth(100);
    m_userLabel->setObjectName("userLabel");

    m_stateIcon->setMenu(m_stateMenu);
    m_stateIcon->setStyleSheet("QPushButton::menu-indicator{image:none;}");
    m_stateIcon->setFixedSize(20, 20);
    m_stateIcon->setObjectName("stateIcon");

    m_stateMenu->addAction(m_onlineAction);
    m_stateMenu->addAction(m_offlineAction);
    m_stateMenu->addAction(m_restartAction);
    m_stateMenu->setFixedWidth(100);
    m_stateMenu->setObjectName("stateMenu");
}

void LocalWidget::m_setLayout()
{
    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(m_userLabel);
    box->addWidget(m_stateIcon, 0, Qt::AlignLeft);
    box->setSpacing(10);
}

void LocalWidget::m_setSignalSlot()
{
    connect(m_onlineAction, &QAction::triggered, this, &LocalWidget::m_setOnline);
    connect(m_offlineAction, &QAction::triggered, this, &LocalWidget::m_setOffline);
    connect(m_restartAction, &QAction::triggered, this, &LocalWidget::m_restart);
}

void LocalWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/localwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void LocalWidget::m_setOnline()
{
    if(!m_online)
        this->m_setState(true);      //由登录状态来控制
}

void LocalWidget::m_setOffline()
{
    if(m_online)
        this->m_setState(false);     //直接离线，不由登录状态来控制
}

/*
 * summary: 手动设置当前登录状态。另外还有setState用于被动设置登录状态，
 *          通过登录结果或网络出现的错误设置相应的登录状态。详细见setState
 * param: state 手动设置的结果
 * ret: 无
 * note: 不论是手动还是被动，当离线时都会发送offline信号，用于改变好友的在线状态
 **/
void LocalWidget::m_setState(bool state)
{
    m_online = state;

    if(m_online) {
        m_stateIcon->setIcon(QIcon(":/localwidget/online"));
        emit online();
    } else {
        m_stateIcon->setIcon(QIcon(":/localwidget/offline"));
        emit offline();
    }
}

void LocalWidget::m_restart()
{
#ifndef EXIT_RESTART
#define EXIT_RESTART 774
#endif
    QApplication::exit(EXIT_RESTART);
}

