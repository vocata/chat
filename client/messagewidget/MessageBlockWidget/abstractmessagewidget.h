#ifndef ABSTRACTMESSAGEWIDGET_H
#define ABSTRACTMESSAGEWIDGET_H

#include <QJsonDocument>
#include <QWidget>

class AbstractMessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractMessageWidget(QWidget *parent = 0);
    virtual void setMessageState(const QString state) = 0;
    virtual void setInfo(const QJsonDocument &json);
    virtual void cancel();
    static QString sizeUnitAdjust(quint64 size);
protected:
    void paintEvent(QPaintEvent *);
};

#endif // ABSTRACTMESSAGEWIDGET_H
