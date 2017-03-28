#include "message/messagetype.h"
#include "imagemessagesentwidget.h"
#include "messagewidget/MessageBlockWidget/imagemessagewidget/imageprogresswidget.h"
#include <QLabel>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QDateTime>
#include <QDebug>

ImageMessageSentWidget::ImageMessageSentWidget(QWidget *parent) : AbstractMessageWidget(parent)
{
    m_imageWidget = new ImageProgressWidget(this);
    m_messageDate = new QLabel(this);
    m_messageState = new QLabel(this);
    m_uploader = new Uploader(REMOTEHOST_ADDRESS, REMOTEHOST_FILE_PORT, this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
}

void ImageMessageSentWidget::setInfo(const QJsonDocument &json)
{
    QVariantMap map = json.object().toVariantMap();
    QString localAccount = map.value("localAccount").toString();
    quint64 messageId = map.value("messageId").toULongLong();

    /* 设置本地完整文件名和远程完整文件名 */
    QFileInfo localFileInfo(map.value("message").toString());
    m_localFileName = localFileInfo.filePath();
    m_remoteFileName = localAccount + "/image/" + QString::number(messageId) + "." + localFileInfo.suffix();

    /* 修改json的内容，去掉文件的路径名，补全完整的文件信息 */
    m_imageWidget->setToolTip(localFileInfo.fileName());
    m_imageWidget->setImage(m_localFileName);
    m_imageWidget->adjustSize();
    map["message"] = localFileInfo.fileName() + ":"
                     + QString::number(m_imageWidget->width()) + ":"
                     + QString::number(m_imageWidget->height());      //本地文件名:图片宽:图片高
    m_json = QJsonDocument::fromVariant(map);

    QDateTime date;
    date.setMSecsSinceEpoch(messageId);
    m_messageDate->setText(date.toString("yyyy.MM.dd\\hh:mm"));

    m_uploader->put(m_localFileName, m_remoteFileName);
}

void ImageMessageSentWidget::m_cancelHandler()
{
    m_imageWidget->updateProgress(0, 0);
    m_messageState->setText(QStringLiteral("未发送"));
}

void ImageMessageSentWidget::m_setAttribution()
{
    m_imageWidget->setObjectName("imageWidget");
    m_messageDate->setObjectName("messageDate");
    m_messageState->setObjectName("messageState");
}

void ImageMessageSentWidget::m_setLayout()
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

void ImageMessageSentWidget::m_setSignalSlot()
{
    connect(m_uploader, &Uploader::uploadProgress, m_imageWidget, &ImageProgressWidget::updateProgress);
    connect(m_uploader, &Uploader::uploadFinish, this, &ImageMessageSentWidget::m_finishHandler);
    connect(m_uploader, &Uploader::transerCancel, this, &ImageMessageSentWidget::m_cancelHandler);
    connect(m_uploader, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
}

void ImageMessageSentWidget::m_finishHandler()
{
    emit finish(Image, m_json);
}

void ImageMessageSentWidget::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    this->m_cancelHandler();
}
