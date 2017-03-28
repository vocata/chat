#include "historywidget.h"
#include "messagewidget/MessageBlockWidget/abstractmessagewidget.h"

#include <QVBoxLayout>
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

HistoryWidget::HistoryWidget(QWidget *parent) : QScrollArea(parent)
{
    m_contentWidget = new QWidget(this); 
    m_space = new QWidget(this);
    m_messageBox = new QVBoxLayout();
    m_animation = new QPropertyAnimation(this->verticalScrollBar(), "value", this);

    m_contentWidget->setObjectName("contentWidget");
    m_space->setFixedHeight(0);
    m_messageBox->setSpacing(0);
    m_messageBox->setMargin(0);
    m_animation->setEasingCurve(QEasingCurve::OutQuad);

    QVBoxLayout *box = new QVBoxLayout(m_contentWidget);
    box->addLayout(m_messageBox);
    box->addWidget(m_space, 0, Qt::AlignTop);
    box->setSpacing(0);
    box->setMargin(0);

    this->setWidget(m_contentWidget);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidgetResizable(true);
    this->verticalScrollBar()->setObjectName("historyScrollBar");
    this->m_setStyleSheet();
}

void HistoryWidget::addMessage(AbstractMessageWidget *message)
{
    m_messageBox->addWidget(message);
    if(m_messageBox->indexOf(message) % 2)
        message->setObjectName("oddWidget");
    else
        message->setObjectName("evenWidget");

    QTimer::singleShot(50, this, &HistoryWidget::m_showNewMessage);
}

void HistoryWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/historywidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void HistoryWidget::m_showNewMessage()
{
    m_animation->setStartValue(this->verticalScrollBar()->value());
    m_animation->setEndValue(this->verticalScrollBar()->maximum());
    m_animation->start();
}

