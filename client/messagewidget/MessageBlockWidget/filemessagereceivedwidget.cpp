#include "message/messagetype.h"
#include "filemessagereceivedwidget.h"
#include <QHBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QVariantMap>
#include <QJsonObject>
#include <QDir>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

#include <QDebug>

FileMessageReceivedWidget::FileMessageReceivedWidget(QWidget *parent) : AbstractMessageWidget(parent)
{
    m_fileIcon = new QPushButton(this);
    m_fileName = new QLabel(this);
    m_fileSize = new QLabel(this);
    m_downloadButton = new QPushButton(this);
    m_saveAsButton = new QPushButton(this);
    m_progressBar = new QProgressBar(this);
    m_cancelButton = new QPushButton(this);
    m_openFileButton = new QPushButton(this);
    m_openDirButton = new QPushButton(this);
    m_messageDate = new QLabel(this);
    m_messageState = new QLabel(this);
    m_downloader = new Downloader(REMOTEHOST_ADDRESS, REMOTEHOST_FILE_PORT, this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}

void FileMessageReceivedWidget::setInfo(const QJsonDocument &json)
{
    QVariantMap map = json.object().toVariantMap();
    QString peerAccount = map.value("localAccount").toString();
    QString localAccount = map.value("peerAccount").toString();
    quint64 messageId = map.value("messageId").toULongLong();
    QString message = map.value("message").toString();
    QString localFileName = message.section(':', 0, -3);
    QString remoteFileName = message.section(':', -2, -2);
    quint64 fileSize = message.section(':', -1, -1).toULongLong();

    m_localFileName = QDir::currentPath() + "/" + localAccount + "/file/" + localFileName;
    m_remoteFileName = peerAccount + "/file/" + remoteFileName;

    /* 设置文件名和文件大小 */
    QDir().mkpath(QDir::currentPath() + "/" + localAccount + "/file/");
    QFontMetrics elideFont(m_fileName->font());
    m_fileName->setText(elideFont.elidedText(localFileName, Qt::ElideMiddle, m_fileName->width()));
    m_fileSize->setText(sizeUnitAdjust(fileSize));
    m_fileIcon->setToolTip(localFileName);
    m_fileName->setToolTip(localFileName);
    m_fileSize->setToolTip(sizeUnitAdjust(fileSize));

    /* 设置文件类型图标 */
    QString suffix = QFileInfo(localFileName).suffix();
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
}

void FileMessageReceivedWidget::m_setAttribution()
{
    m_fileIcon->setCursor(Qt::PointingHandCursor);
    m_fileIcon->setIcon(QIcon(":/filetype/none"));
    m_fileIcon->setIconSize(QSize(55, 55));
    m_fileIcon->setFixedSize(QSize(55, 55));
    m_fileIcon->setObjectName("fileIcon");

    m_fileName->setMaximumWidth(100);
    m_fileName->setObjectName("fileName");

    m_fileSize->setObjectName("fileSize");

    m_downloadButton->setCursor(Qt::PointingHandCursor);
    m_downloadButton->setText(QStringLiteral("下载"));
    m_downloadButton->setFixedSize(40, 20);
    m_downloadButton->setObjectName("downloadButton");

    m_saveAsButton->setCursor(Qt::PointingHandCursor);
    m_saveAsButton->setText(QStringLiteral("另存为"));
    m_saveAsButton->setFixedSize(50, 20);
    m_saveAsButton->setObjectName("saveAsButton");

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

void FileMessageReceivedWidget::m_setLayout()
{
    QHBoxLayout *labelBox = new QHBoxLayout();
    labelBox->addWidget(m_fileName);
    labelBox->addWidget(m_fileSize, 0, Qt::AlignLeft);
    labelBox->setSpacing(10);
    labelBox->setMargin(0);

    QHBoxLayout *buttonBox = new QHBoxLayout();
    buttonBox->addWidget(m_downloadButton);
    buttonBox->addWidget(m_saveAsButton, 0, Qt::AlignLeft);
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

    this->m_noDownloading();
}

void FileMessageReceivedWidget::m_setSignalSlot()
{
    connect(m_downloadButton, &QPushButton::clicked, this, &FileMessageReceivedWidget::m_startDownload);
    connect(m_saveAsButton, &QPushButton::clicked, this, &FileMessageReceivedWidget::m_saveAs);
    connect(m_cancelButton, &QPushButton::clicked, m_downloader, &Downloader::cancel);
    connect(m_openFileButton, &QPushButton::clicked, this, &FileMessageReceivedWidget::m_openFile);
    connect(m_openDirButton, &QPushButton::clicked, this, &FileMessageReceivedWidget::m_openDir);
    connect(m_downloader, &Downloader::downloadProgress, this, &FileMessageReceivedWidget::m_downloadProgressBar);
    connect(m_downloader, &Downloader::downloadFinish, this, &FileMessageReceivedWidget::m_finishHandler);
    connect(m_downloader, &Downloader::transerCancel, this, &FileMessageReceivedWidget::m_cancelHandler);
    connect(m_downloader, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(m_errorHandler(QAbstractSocket::SocketError)));
}

void FileMessageReceivedWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/filemessagereceivedwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void FileMessageReceivedWidget::m_downloading()
{
    m_downloadButton->hide();
    m_saveAsButton->hide();
    m_openDirButton->hide();
    m_openFileButton->hide();
    m_progressBar->show();
    m_cancelButton->show();
}

void FileMessageReceivedWidget::m_downloadDone()
{
    m_progressBar->hide();
    m_cancelButton->hide();
    m_downloadButton->hide();
    m_saveAsButton->hide();
    m_openDirButton->show();
    m_openFileButton->show();
}

void FileMessageReceivedWidget::m_noDownloading()
{
    m_progressBar->hide();
    m_cancelButton->hide();
    m_openDirButton->hide();
    m_openFileButton->hide();
    m_downloadButton->show();
    m_saveAsButton->show();
}

void FileMessageReceivedWidget::m_downloadProgressBar(qint64 bytesReceived, qint64 bytesTotal)
{
    m_progressBar->setRange(0, bytesTotal);
    m_progressBar->setValue(bytesReceived);
}

void FileMessageReceivedWidget::m_startDownload()
{
    this->m_downloading();
    m_downloader->get(m_localFileName, m_remoteFileName);
}

void FileMessageReceivedWidget::m_saveAs()
{
    QString fileName = QFileInfo(m_localFileName).fileName();
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存文件"),
                                                    QString("/home/%1").arg(fileName), "Files (*.*)");
    if(filePath.isEmpty())
        return;
    this->m_downloading();
    m_localFileName = filePath;
    m_downloader->get(filePath, m_remoteFileName);
}

void FileMessageReceivedWidget::m_finishHandler()
{
    this->m_downloadDone();
    m_messageState->setText(QStringLiteral("已下载"));
    connect(m_fileIcon, &QPushButton::clicked, this, &FileMessageReceivedWidget::m_openFile);
}

void FileMessageReceivedWidget::m_cancelHandler()
{
    this->m_noDownloading();
    m_progressBar->setValue(0);
    m_messageState->setText(QStringLiteral("未下载"));
}

void FileMessageReceivedWidget::m_openFile()
{
    QUrl url = QUrl::fromLocalFile(m_localFileName);
    QDesktopServices::openUrl(url);
}

void FileMessageReceivedWidget::m_openDir()
{
    QUrl url = QUrl::fromLocalFile(QFileInfo(m_localFileName).absolutePath());
    QDesktopServices::openUrl(url);
}

void FileMessageReceivedWidget::m_errorHandler(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    this->m_cancelHandler();
}
