#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QScrollArea>

class QVBoxLayout;
class AbstractMessageWidget;
class QPropertyAnimation;
class HistoryWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit HistoryWidget(QWidget *parent = 0);
    void addMessage(AbstractMessageWidget *message);
private:
    QWidget *m_contentWidget;
    QWidget *m_space;
    QVBoxLayout *m_messageBox;
    QPropertyAnimation *m_animation;
    void m_setStyleSheet();
private slots:
    void m_showNewMessage();
};

#endif // HISTORYWIDGET_H
