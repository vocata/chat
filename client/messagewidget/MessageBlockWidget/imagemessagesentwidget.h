#ifndef IMAGEMESSAGESENTWIDGET_H
#define IMAGEMESSAGESENTWIDGET_H

#include "abstractmessagewidget.h"
#include "messagewidget/MessageBlockWidget/filemessagewidget/fileclient.h"
#include <QLabel>
#include <QAbstractSocket>

class ImageProgressWidget;
class Uploader;
class ImageMessageSentWidget : public AbstractMessageWidget
{
    Q_OBJECT
public:
    explicit ImageMessageSentWidget(QWidget *parent = 0);
    void setMessageState(const QString state);
    void setInfo(const QJsonDocument &json);
    void cancel();
signals:
    void finish(quint32 type, QJsonDocument json);
private:
    ImageProgressWidget *m_imageWidget;
    QLabel *m_messageDate;
    QLabel *m_messageState;
    Uploader *m_uploader;
    QString m_localFileName;
    QString m_remoteFileName;
    QJsonDocument m_json;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_cancelHandler();
private slots:
    void m_finishHandler();
    void m_errorHandler(QAbstractSocket::SocketError error);
};

inline void ImageMessageSentWidget::setMessageState(const QString state) { m_messageState->setText(state); }
inline void ImageMessageSentWidget::cancel() { m_uploader->cancel(); }

#endif // IMAGEMESSAGESENTWIDGET_H
