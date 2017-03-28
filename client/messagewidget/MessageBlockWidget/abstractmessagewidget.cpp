#include "abstractmessagewidget.h"

#include <QPainter>
#include <QStyleOption>

AbstractMessageWidget::AbstractMessageWidget(QWidget *parent) : QWidget(parent)
{

}

void AbstractMessageWidget::setMessageState(const QString state)
{
    Q_UNUSED(state);
}

void AbstractMessageWidget::setInfo(const QJsonDocument &json)
{
    Q_UNUSED(json);
}

void AbstractMessageWidget::cancel()
{

}

QString AbstractMessageWidget::sizeUnitAdjust(quint64 size)
{
    if(size < 1000)
        return QString::number(size) + "B";
    else if(size < 1000 * 1000)
        return QString::number((double)size / 1000.0, 'f', 1) + "KB";
    else if(size < 1000 * 1000 * 1000)
        return QString::number((double)size / 1000.0 / 1000.0, 'f', 1) + "MB";
    else
        return QString::number(size / 1000.0 / 1000.0 / 1000.0, 'f', 1) + "GB";
}

void AbstractMessageWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

