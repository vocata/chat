#include "message/messagetype.h"
#include "filemessagesentwidget.h"
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QUrl>

#include <QDebug>

FileMessageSentWidget::FileMessageSentWidget(QWidget *parent) : AbstractMessageWidget(parent)
{
    m_fileIcon = new QPushButton(this);
    m_fileName = new QLabel(this);
    m_fileSize = new QLabel(this);
    m_reuploadButton = new QPushButton(this);
    m_progressBar = new QProgressBar(this);
    m_cancelButton = new QPushButton(this);
    m_openFileButton = new QPushButton(this);
    m_openDirButton = new QPushButton(this);
    m_messageDate = new QLabel(this);
    m_messageState = new QLabel(this);
    m_uploader = new Uploader(REMOTEHOST_ADDRESS, REMOTEHOST_FILE_PORT, this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}

void FileMessageSentWidget::setInfo(const QJsonDocument &json)
{
    QVariantMap map = json.object().toVariantMap();
    QString localAccount = map.value("localAccount").toString();
    quint64 messageId = map.value("messageId").toULongLong();
    QFileInfo localFileInfo(map.value("message").toString());
    QString remoteFileName = QString::number(QDateTime::currentMSecsSinceEpoch()) + "." + localFileInfo.suffix();

    /* 设置本地完整文件名和远程完整文件名 */
    m_localFileName = localFileInfo.filePath();
    m_remoteFileName = localAccount + "/file/" + remoteFileName;

    /* 修改json的内容，去掉文件的路径名，补全完整的文件信息 */
    map["message"] = localFileInfo.fileName() + ":" + remoteFileName+ ":" + QString::number(localFileInfo.size());      //本地文件名:远程文件名:文件大小
    m_json = QJsonDocument::fromVariant(map);

    /* 设置文件名和文件大小 */
    QFontMetrics elideFont(m_fileName->font());
    m_fileName->setText(elideFont.elidedText(localFileInfo.fileName(), Qt::ElideMiddle, m_fileName->width()));
    m_fileSize->setText(sizeUnitAdjust(localFileInfo.size()));
    m_fileIcon->setToolTip(localFileInfo.fileName());
    m_fileName->setToolTip(localFileInfo.fileName());
    m_fileSize->setToolTip(sizeUnitAdjust(localFileInfo.size()));

    /* 设置文件类型图标 */
    QString suffix = localFileInfo.suffix();
    QDir dir(":/filetype");
    QStringList fileTypeList = dir.entryList(QDir::Files);
    for(const auto &fileType: fileTypeList) {
        QStringList fileSuffix = fileType.split(',');
        if(fileSuffix.contains(suffix)) {
            m_fileIcon->setIcon(QIcon(QString(":/filetype/%1").arg(fileType)));
        }
    }

    QDateTime date;
    date.setMSecsSinceEpoch(messageId);
    m_messageDate->setText(date.toString("yyyy.MM.dd\\hh:mm"));

    m_uploader->put(m_localFileName, m_remoteFileName);
}

void FileMessageSentWidget::m_setAttribution()
{
    m_fileIcon->setCursor(Qt::PointingHandCursor);
    m_fileIcon->setIcon(QIcon(":/filetype/none"));
    m_fileIcon->setIconSize(QSize(55, 55));
    m_fileIcon->setFixedSize(QSize(55, 55));
    m_fileIcon->setObjectName("fileIcon");

    m_fileName->setMaximumWidth(100);
    m_fileName->setObjectName("fileName");

    m_fileSize->setObjectName("fileSize");

    m_reuploadButton->setCursor(Qt::PointingHandCursor);
    m_reuploadButton->setText(QStringLiteral("重试"));
    m_reuploadButton->setFixedSize(40, 20);
    m_reuploadButton->setObjectName("reuploadButton");

    m_progressBar->setFormat("");
    m_progressBar->setValue(0);
    m_progressBar->setFixedSize(140, 5);
    m_progressBar->setObjectName("progressBar");

    m_cancelButton->setCursor(Qt::PointingHandCursor);
    m_cancelButton->setToolTip(QStringLiteral("取消"));
    m_cancelButton->setIcon(QIcon(":/messagewidget/cancel"));
    m_cancelButton->setIconSize(QSize(18, 18));
    m_cancelButton->setFixedSize(QSize(20, 20));
    m_cancelButton->setObjectName("cancelButton");

    m_openFileButton->setCursor(Qt::PointingHandCursor);
    m_openFileButton->setText(QStringLiteral("打开"));
    m_openFileButton->setFixedSize(40, 20);
    m_openFileButton->setObjectName("openFileButton");

    m_openDirButton->setCursor(Qt::PointingHandCursor);
    m_openDirButton->setText(QStringLiteral("打开文件夹"));
    m_openDirButton->setFixedSize(80, 20);
    m_openDirButton->setObjectName("openDirButton");

    m_messageDate->setObjectName("messageDate");
    m_messageState->setObjectName("messageState");
}

void FileMessageSentWidget::m_setLayout()
{
    QHBoxLayout *labelBox = new QHBoxLayout();
    labelBox->addWidget(m_fileName);
    labelBox->addWidget(m_fileSize, 0, Qt::AlignLeft);
    labelBox->setSpacing(10);
    labelBox->setMargin(0);

    QHBoxLayout *buttonBox = new QHBoxLayout();
    buttonBox->addWidget(m_reuploadButton, 0, Qt::AlignLeft);
    buttonBox->addWidget(m_progressBar);
    buttonBox->addWidget(m_cancelButton, 0, Qt::AlignLeft);
    buttonBox->addWidget(m_openFileButton);
    buttonBox->addWidget(m_openDirButton, 0, Qt::AlignLeft);
    buttonBox->setSpacing(0);
    buttonBox->setMargin(0);

    QVBoxLayout *fileBox = new QVBoxLayout();
    fileBox->addLayout(labelBox);
    fileBox->addLayout(buttonBox);
    fileBox->setSpacing(10);
    fileBox->setMargin(0);

    QHBoxLayout *leftBox = new QHBoxLayout();
    leftBox->addWidget(m_fileIcon);
    leftBox->addLayout(fileBox);
    leftBox->setSpacing(10);
    leftBox->setMargin(0);

    QVBoxLayout *rightBox = new QVBoxLayout();
    rightBox->addWidget(m_messageDate);
    rightBox->addWidget(m_messageState, 0, Qt::AlignRight);
    rightBox->setSpacing(10);
    rightBox->setMargin(0);

    QHBoxLayout *box = new QHBoxLayout(this);
    box->addLayout(leftBox);
    box->addLayout(rightBox);
    box->setAlignment(rightBox, Qt::AlignTop | Qt::AlignRight);
    box->setMargin(10);

    this->m_uploading();
}

void FileMessageSentWidget::m_setSignalSlot()
{
    connect(m_fileIcon, &QPushButton::clicked, this, &FileMessageSentWidget::m_openFile);
    connect(m_reuploadButton, &QPushButton::clicked, this, &FileMessageSentWidget::m_reupload);
    connect(m_cancelButton, &QPushButton::clicked, m_uploader, &Uploader::cancel);
    connect(m_openFileButton, &QPushButton::clicked, this, &FileMessageSentWidget::m_openFile);
    connect(m_openDirButton, &QPushButton::clicked, this, &FileMessageSentWidget::m_openDir);
    connect(m_uploader, &Uploader::uploadProgress, this, &FileMessageSentWidget::m_updateProgressBar);
    connect(m_uploader, &Uploader::uploadFinish, this, &FileMessageSentWidget::m_finishHandler);
    connect(m_uploader, &Uploader::transerCancel, this, &FileMessageSentWidget::m_cancelHandler);
    connect(m_uploader, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
}

void FileMessageSentWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/filemessagesentwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void FileMessageSentWidget::m_uploading()           //正在发送的情况
{
    m_reuploadButton->hide();
    m_openDirButton->hide();
    m_openFileButton->hide();
    m_progressBar->show();
    m_cancelButton->show();
}

void FileMessageSentWidget::m_noUploading()         //发送失败的情况
{
    m_progressBar->hide();
    m_cancelButton->hide();
    m_openDirButton->hide();
    m_openFileButton->hide();
    m_reuploadButton->show();
}

void FileMessageSentWidget::m_uploadDone()          //发送成功的情况
{
    m_reuploadButton->hide();
    m_progressBar->hide();
    m_cancelButton->hide();
    m_openDirButton->show();
    m_openFileButton->show();
}

void FileMessageSentWidget::m_updateProgressBar(qint64 bytesSent, qint64 bytesTotal)
{
    m_progressBar->setRange(0, bytesTotal);
    m_progressBar->setValue(bytesSent);
//    qDebug() << bytesSent;
}

void FileMessageSentWidget::m_finishHandler()
{  
    this->m_uploadDone();
    emit finish(File, m_json);
}

void FileMessageSentWidget::m_reupload()
{
    QFileInfo localFileInfo(m_localFileName);
    if(!localFileInfo.exists()) {
        m_messageState->setText(QStringLiteral("文件不存在"));
        return;
    }
    /* 更新一系列远程文件信息和发送的信息 */
    QVariantMap map = m_json.object().toVariantMap();
    QString localAccount = map.value("localAccount").toString();
    QString remoteFileName = QString::number(QDateTime::currentMSecsSinceEpoch()) + "." + localFileInfo.suffix();
    m_remoteFileName = localAccount + "/file/" + remoteFileName;        //远程文件信息

    map["message"] = localFileInfo.fileName() + ":" + remoteFileName + ":" + QString::number(localFileInfo.size()); //更新发送给对方的远程文件信息
    m_json = QJsonDocument::fromVariant(map);

    this->m_uploading();
    m_uploader->put(m_localFileName, m_remoteFileName);
    m_messageState->setText(QStringLiteral("正在发送..."));
}

void FileMessageSentWidget::m_cancelHandler()
{
    this->m_noUploading();
    m_progressBar->setValue(0);
    m_messageState->setText(QStringLiteral("未发送"));
}

void FileMessageSentWidget::m_openFile()
{
    QUrl url = QUrl::fromLocalFile(m_localFileName);
    QDesktopServices::openUrl(url);
}

void FileMessageSentWidget::m_openDir()
{
    QUrl url = QUrl::fromLocalFile(QFileInfo(m_localFileName).absolutePath());
    QDesktopServices::openUrl(url);
}

void FileMessageSentWidget::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    this->m_cancelHandler();
}

