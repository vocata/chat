#ifndef PREDIALOG_H
#define PREDIALOG_H

#include <QDialog>
#include <QAbstractSocket>

class QLineEdit;
class QTcpSocket;

enum LoginStatus: quint32 { LoginSuccess = 20, PasswdError , NoAccount };

class PreDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PreDialog(QTcpSocket *socket, QWidget *parent = 0);
    QMap<QString, QString> getLoginedInfo();

private:
    QPushButton *m_accountIcon;
    QPushButton *m_passwdIcon;
    QLineEdit *m_accountLineEdit;
    QLineEdit *m_passwdLineEdit;
    QPushButton *m_loginButton;
    QPushButton *m_registerButton;
    QPushButton *m_findPasswdButton;

    QTcpSocket *m_tcpSocket;
    QString m_account;
    QString m_passwd;
    QString m_user;
    QByteArray m_request;
    bool m_isLogined;
    const qint32 Width = 400;
    const qint32 Height = 250;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
    void m_setLineEditEnable(bool enable);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void m_login();
    void m_register();
    void m_sendRequest();
    void m_readReply();
    void m_errorHandler(QAbstractSocket::SocketError error);

};

#endif // PREDIALOG_H
