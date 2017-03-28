#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QJsonDocument>
#include <QGroupBox>

class QTextEdit;
class QPushButton;
class EmojiWidget;

class ChatWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit ChatWidget(QWidget *parent = 0);
    void setAccount(const QString &localAccount, const QString &peerAccount);
    void setState(bool online);
signals:
    void sendMessage(quint32 type, const QJsonDocument &msg);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private:
    QTextEdit *m_chatTextEdit;
    QPushButton *m_emojiIcon;
    QPushButton *m_photoIcon;
    QPushButton *m_fileIcon;
    QPushButton *m_sendButton;
    EmojiWidget *m_emojiWidget;
    QString m_localAccount;
    QString m_peerAccount;
    bool m_online;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
private slots:
    void m_insertEmoji(const QString &path);
    void m_sendNolmalMessage();
    void m_sendImageMessage();
    void m_sendFileMessage();
};

inline void ChatWidget::setState(bool online) { m_online = online; }

#endif // CHATWIDGET_H
