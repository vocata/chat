#include "message/messagetype.h"
#include "imagemessagereceivewidget.h"
#include "messagewidget/MessageBlockWidget/imagemessagewidget/imageprogresswidget.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QVBoxLayout>

ImageMessageReceiveWidget::ImageMessageReceiveWidget(QWidget *parent) : AbstractMessageWidget(parent)
{
    m_imageWidget = new ImageProgressWidget(this);
    m_messageDate = new QLabel(this);
    m_messageState = new QLabel(this);
    m_downloader = new Downloader(REMOTEHOST_ADDRESS, REMOTEHOST_FILE_PORT, this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
}

void ImageMessageReceiveWidget::setInfo(const QJsonDocument &json)
{
    qDebug() << json;
    QVariantMap map = json.object().toVariantMap();
    QString peerAccount = map.value("localAccount").toString();
    QString localAccount = map.value("peerAccount").toString();
    quint64 messageId = map.value("messageId").toULongLong();
    QString message = map.value("message").toString();
    QString localFileName = message.section(':', 0, -3);
    QString remoteFileName = QString::number(messageId) + "." + QFileInfo(localFileName).suffix();
    qint32 width = message.section(':', -2, -2).toInt();
    qint32 height = message.section(':', -1, -1).toInt();

    QDir().mkpath(QDir::currentPath() + "/" + localAccount + "/image/");
    m_localFileName = QDir::currentPath() + "/" + localAccount + "/image/" + localFileName;
    m_remoteFileName = peerAccount + "/image/" + remoteFileName;

    m_imageWidget->setToolTip(localFileName);
    m_imageWidget->setAspectRatio(width, height);

    QDateTime date;
    date.setMSecsSinceEpoch(messageId);
    m_messageDate->setText(date.toString("yyyy.MM.dd\\hh:mm"));

    m_downloader->get(m_localFileName, m_remoteFileName);
}

void ImageMessageReceiveWidget::m_setAttribution()
{
    m_imageWidget->setObjectName("imageWidget");
    m_messageDate->setObjectName("messageDate");
    m_messageState->setObjectName("messageState");
}

void ImageMessageReceiveWidget::m_setLayout()
{
    QVBoxLayout *rightBox = new QVBoxLayout();
    rightBox->addWidget(m_messageDate);
    rightBox->addWidget(m_messageState, 0, Qt::AlignRight);
    rightBox->setSpacing(10);
    rightBox->setMargin(0);

    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(m_imageWidget, 0, Qt::AlignTop);
    box->addLayout(rightBox);
    box->setAlignment(rightBox, Qt::AlignTop | Qt::AlignRight);
    box->setMargin(10);
}

void ImageMessageReceiveWidget::m_setSignalSlot()
{
    connect(m_downloader, &Downloader::downloadProgress, m_imageWidget, &ImageProgressWidget::updateProgress);
    connect(m_downloader, &Downloader::downloadFinish, this, &ImageMessageReceiveWidget::m_downloadFinish);
    connect(m_downloader, &Downloader::transerCancel, this, &ImageMessageReceiveWidget::m_cancelHandler);
    connect(m_downloader, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
}

void ImageMessageReceiveWidget::m_cancelHandler()
{
    m_imageWidget->updateProgress(0, 0);
    m_messageState->setText(QStringLiteral("未下载"));
}

void ImageMessageReceiveWidget::m_downloadFinish()
{
    m_imageWidget->setImage(m_localFileName);
    m_messageState->setText(QStringLiteral("已下载"));
}

void ImageMessageReceiveWidget::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    this->m_cancelHandler();
}
