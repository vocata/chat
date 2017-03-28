#ifndef MESSAGESTACKEDWIDGET_H
#define MESSAGESTACKEDWIDGET_H

#include <QStackedWidget>
#include <QMap>

class StartWidget;
class MessageWidget;
class QSound;

class MessageStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit MessageStackedWidget(QWidget *parent = 0);

    void setLoginedInfo(const QMap<QString, QString> &info);
    void addReceiveMessage(quint32 type, const QJsonDocument &json);
    void updateFriendList(const QJsonDocument &doc);
    void clearFriendState();
    void setSendMessageState(const QString &peerAccount, quint64 messageId);
    void setState(bool online);

signals:
    void sendMessage(const QByteArray &message);
    void newMessageReceive(const QString &account);

public slots:
    void setCurrentAccountWidget(const QString &account);

private:
    StartWidget *m_startWidget;
    QMap<QString, QString> m_loginInfo;
    QMap<QString, MessageWidget *> m_friends;
    QString m_currentMessageWidgetAccount;
    QSound *m_tips;
};

inline void MessageStackedWidget::setLoginedInfo(const QMap<QString, QString> &info) { m_loginInfo = info; }

#endif // MESSAGESTACKEDWIDGET_H
