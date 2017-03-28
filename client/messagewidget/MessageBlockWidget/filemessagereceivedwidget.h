#ifndef FILEMESSAGERECEIVEDWIDGET_H
#define FILEMESSAGERECEIVEDWIDGET_H

#include "abstractmessagewidget.h"
#include "messagewidget/MessageBlockWidget/filemessagewidget/fileclient.h"
#include <QLabel>
#include <QAbstractSocket>

class QPushButton;
class QProgressBar;
class FileMessageReceivedWidget : public AbstractMessageWidget
{
    Q_OBJECT
public:
    explicit FileMessageReceivedWidget(QWidget *parent = 0);
    void setInfo(const QJsonDocument &json);
    void setMessageState(const QString state);
    void cancel();
signals:

private:
    QPushButton *m_fileIcon;
    QLabel *m_fileName;
    QLabel *m_fileSize;
    QPushButton *m_downloadButton;
    QPushButton *m_saveAsButton;
    QProgressBar *m_progressBar;
    QPushButton *m_cancelButton;
    QPushButton *m_openFileButton;
    QPushButton *m_openDirButton;
    QLabel *m_messageDate;
    QLabel *m_messageState;
    Downloader *m_downloader;
    QString m_localFileName;
    QString m_remoteFileName;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
    void m_downloading();
    void m_downloadDone();
    void m_noDownloading();
private slots:
    void m_downloadProgressBar(qint64 bytesReceived, qint64 bytesTotal);
    void m_startDownload();
    void m_saveAs();
    void m_finishHandler();
    void m_cancelHandler();
    void m_openFile();
    void m_openDir();
    void m_errorHandler(QAbstractSocket::SocketError error);
};

inline void FileMessageReceivedWidget::setMessageState(const QString state) { m_messageState->setText(state); }
inline void FileMessageReceivedWidget::cancel() { m_downloader->cancel(); }

#endif // FILEMESSAGERECEIVEDWIDGET_H
