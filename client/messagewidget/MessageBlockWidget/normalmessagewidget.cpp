#include "message/messagetype.h"
#include "normalmessagewidget.h"

#include <QDateTime>
#include <QJsonObject>
#include <QLabel>
#include <QTextEdit>
#include <QTextDocument>
#include <QTimer>
#include <QVBoxLayout>

#include <QDebug>

NormalMessageWidget::NormalMessageWidget(QWidget *parent) : AbstractMessageWidget(parent)
{
    m_messageEdit = new QTextEdit(this);
    m_messageDate = new QLabel(this);
    m_messageState = new QLabel(this);
    m_timer = new QTimer(this);

    this->m_setAttribution();
    this->m_setLayout();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}

void NormalMessageWidget::setInfo(const QJsonDocument &json)
{
    QVariantMap map = json.object().toVariantMap();
    quint64 messageId = map.value("messageId").toULongLong();
    QString message = map.value("message").toString();

    QDateTime date;
    date.setMSecsSinceEpoch(messageId);
    m_messageDate->setText(date.toString("yyyy.MM.dd\\hh:mm"));
    m_messageEdit->setText(message);
    this->m_autoAdjustHeight();
    emit finish(Message, json);
}

void NormalMessageWidget::startTiming()
{
    m_timer->start();
}

void NormalMessageWidget::m_setAttribution()
{
    QFont font;
    font.setPointSize(12);
    m_messageEdit->setFont(font);
    m_messageEdit->setReadOnly(true);
    m_messageEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    m_messageEdit->setLineWrapMode(QTextEdit::FixedPixelWidth);
    m_messageEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_messageEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_messageEdit->setLineWrapColumnOrWidth(350);
    m_messageEdit->setFixedWidth(350);
    m_messageEdit->setObjectName("messageEdit");

    m_messageDate->setObjectName("messageDate");
    m_messageState->setObjectName("messageState");
    m_timer->setInterval(1800000);
}

void NormalMessageWidget::m_setLayout()
{
    QVBoxLayout *rightBox = new QVBoxLayout();
    rightBox->addWidget(m_messageDate);
    rightBox->addWidget(m_messageState, 0, Qt::AlignRight);
    rightBox->setSpacing(10);
    rightBox->setMargin(0);

    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(m_messageEdit, 0, Qt::AlignTop);
    box->addLayout(rightBox);
    box->setAlignment(rightBox, Qt::AlignTop | Qt::AlignRight);
    box->setSpacing(60);
    box->setMargin(10);
}

void NormalMessageWidget::m_setSignalSlot()
{
    connect(m_timer, &QTimer::timeout, this, &NormalMessageWidget::m_setSendFailuer);
}

void NormalMessageWidget::m_autoAdjustHeight()
{
    qint32 h = m_messageEdit->document()->size().toSize().height();
    m_messageEdit->setFixedHeight(h + 2);
}

void NormalMessageWidget::m_setSendFailuer()
{
    if(m_messageState->text() == QStringLiteral("正在发送..."))
        m_messageState->setText(QStringLiteral("消息未发送"));
    m_timer->stop();
}


