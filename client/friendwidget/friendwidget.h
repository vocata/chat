#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QMap>
#include <QScrollArea>

class QVBoxLayout;
class FriendButton;
class QButtonGroup;

class FriendWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit FriendWidget(QWidget *parent = 0);
    void updateFriend(const QStringList &allAccountList, const QStringList &onlineAccountList);
    void clearState();
    void setPendingReadFlag(const QString &account);

signals:
    void accountClicked(const QString &account);

private:
    QMap<QString, FriendButton *> m_friends;

    QWidget *m_contentWidget;
    QButtonGroup *m_buttonGroup;
    QWidget *m_space;
    QVBoxLayout *m_buttonBox;
};

#endif // FRIENDWIDGET_H
