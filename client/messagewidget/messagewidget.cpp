#include "message/messagetype.h"
#include "messagewidget.h"
#include "messagewidget/statewidget/statewidget.h"
#include "messagewidget/historywidget/historywidget.h"
#include "messagewidget/chatwidget/chatwidget.h"
#include "messagewidget/MessageBlockWidget/normalmessagewidget.h"
#include "messagewidget/MessageBlockWidget/imagemessagesentwidget.h"
#include "messagewidget/MessageBlockWidget/imagemessagereceivewidget.h"
#include "messagewidget/MessageBlockWidget/filemessagesentwidget.h"
#include "messagewidget/MessageBlockWidget/filemessagereceivedwidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QJsonObject>

#include <QDebug>

MessageWidget::MessageWidget(QWidget *parent) : QWidget(parent)
{
    m_stateWidget = new StateWidget(this);
    m_historyWidget = new HistoryWidget(this);
    m_chatWidget = new ChatWidget(this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
}

void MessageWidget::addReceiveMessage(quint32 type, const QJsonDocument &json)
{
    switch(type) {
    case Message: this->m_addNormalReceiveMessage(json); break;
    case Image: this->m_addImageReceiveMessage(json); break;
    case File: this->m_addFileReceiveMessage(json); break;
    }
}

void MessageWidget::addSendMessage(quint32 type, const QJsonDocument &json)
{
    switch(type) {
    case Message: this->m_addNormalSendMessage(json); break;
    case Image: this->m_addImageSendMessage(json); break;
    case File: this->m_addFileSendMessage(json); break;
    }
}

void MessageWidget::setSendMessageState(quint64 messageId)
{
    AbstractMessageWidget *widget = m_pendingAckMessage.value(messageId);
    m_pendingAckMessage.remove(messageId);
    widget->setMessageState(QStringLiteral("已发送"));
}

void MessageWidget::setState(bool online)
{
    m_chatWidget->setState(online);
    if(!online) {
        for(auto pendingAckMessage: m_pendingAckMessage) {
            pendingAckMessage->cancel();
        }
    }
}

void MessageWidget::m_setAttribution()
{
    m_stateWidget->setFixedHeight(65);
    m_stateWidget->setObjectName("stateWidget");
    m_historyWidget->setObjectName("historyWidget");
    m_chatWidget->setObjectName("chatWidget");
}

void MessageWidget::m_setLayout()
{
    QVBoxLayout *box = new QVBoxLayout(this);
    box->addWidget(m_stateWidget);
    box->addWidget(m_historyWidget);
    box->addWidget(m_chatWidget, 0, Qt::AlignBottom);
    box->setSpacing(0);
    box->setMargin(0);
}

void MessageWidget::m_setSignalSlot()
{
    connect(m_chatWidget, &ChatWidget::sendMessage, this, &MessageWidget::addSendMessage);
}

void MessageWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/messagewidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

//发送
void MessageWidget::m_addNormalSendMessage(const QJsonDocument &json)
{
    NormalMessageWidget *widget = new NormalMessageWidget(this);
    connect(widget, &NormalMessageWidget::finish, this, &MessageWidget::m_sendMessage);

    quint64 messageId = json.object().toVariantMap().value("messageId").toULongLong();
    m_pendingAckMessage.insert(messageId, widget);

    widget->setMessageState(QStringLiteral("正在发送..."));
    widget->setInfo(json);
    widget->startTiming();              //设置信息发送超时
    m_historyWidget->addMessage(widget);
}

void MessageWidget::m_addImageSendMessage(const QJsonDocument &json)
{
    ImageMessageSentWidget *widget = new ImageMessageSentWidget(this);
    connect(widget, &ImageMessageSentWidget::finish, this, &MessageWidget::m_sendMessage);

    quint64 messageId = json.object().toVariantMap().value("messageId").toULongLong();
    m_pendingAckMessage.insert(messageId, widget);

    widget->setMessageState(QStringLiteral("正在发送..."));
    widget->setInfo(json);
    m_historyWidget->addMessage(widget);
}

void MessageWidget::m_addFileSendMessage(const QJsonDocument &json)
{
    FileMessageSentWidget *widget = new FileMessageSentWidget(this);
    connect(widget, &FileMessageSentWidget::finish, this, &MessageWidget::m_sendMessage);

    quint64 messageId = json.object().toVariantMap().value("messageId").toULongLong();
    m_pendingAckMessage.insert(messageId, widget);

    widget->setMessageState(QStringLiteral("正在发送..."));
    widget->setInfo(json);
    m_historyWidget->addMessage(widget);
}


//*********分割线***********

//接收
void MessageWidget::m_addNormalReceiveMessage(const QJsonDocument &json)
{
    NormalMessageWidget *widget = new NormalMessageWidget(this);
    widget->setMessageState(QStringLiteral("已接收"));
    widget->setInfo(json);
    m_historyWidget->addMessage(widget);
}

void MessageWidget::m_addImageReceiveMessage(const QJsonDocument &json)
{
    ImageMessageReceiveWidget *widget = new ImageMessageReceiveWidget(this);
    widget->setMessageState(QStringLiteral("未下载"));
    widget->setInfo(json);
    m_historyWidget->addMessage(widget);
}

void MessageWidget::m_addFileReceiveMessage(const QJsonDocument &json)
{
    FileMessageReceivedWidget *widget = new FileMessageReceivedWidget(this);
    widget->setMessageState(QStringLiteral("未下载"));
    widget->setInfo(json);
    m_historyWidget->addMessage(widget);
}

void MessageWidget::m_sendMessage(quint32 type, const QJsonDocument &json)
{
    QByteArray message;
    QDataStream out(&message, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << type << json.toJson();

    out.device()->seek(0);
    quint32 size = message.size();
    out << size;

    emit sendMessage(message);
}

