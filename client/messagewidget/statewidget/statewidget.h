#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include <QGroupBox>
#include <QLabel>

class StateWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = 0);
    void setUser(const QString &user);
    QString getUser();
    void setFriendState(bool state);
private:
    QLabel *m_userLabel;
    QLabel *m_stateLabel;
    QLabel *m_stateIcon;

    void m_setAttribution();
    void m_setLayout();
};
inline void StateWidget::setUser(const QString &user) { m_userLabel->setText(user); }
inline QString StateWidget::getUser() { return m_userLabel->text(); }
#endif // STATEWIDGET_H
