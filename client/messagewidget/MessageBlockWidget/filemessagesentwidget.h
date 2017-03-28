#ifndef FILEMESSAGESENTWIDGET_H
#define FILEMESSAGESENTWIDGET_H

#include "abstractmessagewidget.h"
#include "messagewidget/MessageBlockWidget/filemessagewidget/fileclient.h"
#include <QJsonDocument>
#include <QLabel>
#include <QAbstractSocket>

class QPushButton;
class QProgressBar;
class FileMessageSentWidget : public AbstractMessageWidget
{
    Q_OBJECT
public:
    explicit FileMessageSentWidget(QWidget *parent = 0);
    void setMessageState(const QString state);
    void setInfo(const QJsonDocument &json);
    void cancel();
signals:
    void finish(quint32 type, QJsonDocument json);
private:
    QPushButton *m_fileIcon;
    QLabel *m_fileName;
    QLabel *m_fileSize;
    QPushButton *m_reuploadButton;
    QProgressBar *m_progressBar;
    QPushButton *m_cancelButton;
    QPushButton *m_openFileButton;
    QPushButton *m_openDirButton;
    QLabel *m_messageDate;
    QLabel *m_messageState;
    QJsonDocument m_json;       //将要发送的文件信息
    Uploader *m_uploader;
    QString m_localFileName;
    QString m_remoteFileName;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
    void m_uploading();
    void m_noUploading();
    void m_uploadDone();
private slots:
    void m_updateProgressBar(qint64 bytesSent, qint64 bytesTotal);
    void m_finishHandler();
    void m_reupload();
    void m_cancelHandler();
    void m_openFile();
    void m_openDir();
    void m_errorHandler(QAbstractSocket::SocketError error);
};

inline void FileMessageSentWidget::setMessageState(const QString state) { m_messageState->setText(state); }
inline void FileMessageSentWidget::cancel() { m_uploader->cancel(); }

#endif // FILEMESSAGESENTWIDGET_H
