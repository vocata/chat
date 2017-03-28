#ifndef FRIENDBUTTON_H
#define FRIENDBUTTON_H

#include <QPushButton>

class QLabel;

class FriendButton : public QPushButton
{
    Q_OBJECT
public:
    explicit FriendButton(QWidget *parent = 0);
    void setInfo(const QString &account, const QString &user, bool online);
    QString getAccount();
    void setAccountAndUser(const QString &account, const QString &user);
    QString getUser();
    bool getState();
    void setState(bool online);
    void setPendingReadFlag();

signals:
    void doubleClicked(const QString &account);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

private:
    QString m_account;
    QString m_user;
    bool m_online;

    QLabel *m_stateIcon;
    QLabel *m_userLabel;
    QLabel *m_stateLabel;
    QLabel *m_pendingReadFlag;

    void m_setAttribution();
    void m_setLayout();
    void m_setStyleSheet();
};

inline QString FriendButton::getAccount() { return m_account; }
inline QString FriendButton::getUser() { return m_user; }
inline bool FriendButton::getState() { return m_online; }

#endif // FRIENDBUTTON_H
