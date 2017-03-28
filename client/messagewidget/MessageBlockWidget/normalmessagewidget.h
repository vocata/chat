#ifndef NORMALMESSAGEWIDGET_H
#define NORMALMESSAGEWIDGET_H

#include "abstractmessagewidget.h"
#include <QJsonDocument>
#include <QLabel>

class QTextEdit;
class NormalMessageWidget : public AbstractMessageWidget
{
    Q_OBJECT
public:
    explicit NormalMessageWidget(QWidget *parent = 0);
    void setInfo(const QJsonDocument &json);
    void setMessageState(const QString state);
    void cancel();
    void startTiming();

signals:
    void finish(quint32 type, QJsonDocument json);

private:
    QTextEdit *m_messageEdit;
    QLabel *m_messageDate;
    QLabel *m_messageState;
    QTimer *m_timer;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_autoAdjustHeight();
private slots:
    void m_setSendFailuer();
};

inline void NormalMessageWidget::setMessageState(const QString state) { m_messageState->setText(state); }
inline void NormalMessageWidget::cancel() { this->m_setSendFailuer(); }

#endif // NORMALMESSAGEWIDGET_H
