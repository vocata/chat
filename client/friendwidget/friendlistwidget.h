#ifndef FRIENDLISTWIDGET_H
#define FRIENDLISTWIDGET_H

#include <QWidget>

class LocalWidget;
class FriendWidget;

class FriendListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FriendListWidget(QWidget *parent = 0);
    void setLoginedInfo(const QMap<QString, QString> &info);
    void updateFriendList(const QJsonDocument &doc);
    void setPendingReadFlag(const QString &account);
    void setState(bool state);

signals:
    void readyChat(const QString &account);
    void online();
    void offline();

private:
    LocalWidget *m_localWidget;
    FriendWidget *m_friendScrollArea;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
};

#endif // FRIENDLISTWIDGET_H
