#ifndef IMAGEMESSAGERECEIVEWIDGET_H
#define IMAGEMESSAGERECEIVEWIDGET_H

#include "abstractmessagewidget.h"
#include "messagewidget/MessageBlockWidget/filemessagewidget/fileclient.h"
#include <QLabel>
#include <QAbstractSocket>

class ImageProgressWidget;
class ImageMessageReceiveWidget : public AbstractMessageWidget
{
    Q_OBJECT
public:
    explicit ImageMessageReceiveWidget(QWidget *parent = 0);
    void setInfo(const QJsonDocument &json);
    void setMessageState(const QString state);
    void cancel();
private:
    ImageProgressWidget *m_imageWidget;
    QLabel *m_messageDate;
    QLabel *m_messageState;
    Downloader *m_downloader;
    QString m_localFileName;
    QString m_remoteFileName;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_cancelHandler();
private slots:
    void m_downloadFinish();
    void m_errorHandler(QAbstractSocket::SocketError error);
};

inline void ImageMessageReceiveWidget::setMessageState(const QString state) { m_messageState->setText(state); }
inline void ImageMessageReceiveWidget::cancel() { m_downloader->cancel(); }

#endif // IMAGEMESSAGERECEIVEWIDGET_H
