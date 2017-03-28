#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

class QLineEdit;
class QTcpSocket;

enum RegisterStatus: quint32 { RegisterSuccess = 30, RegisterFailure };

class RegisterDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget *parent = 0);

signals:

private:
    QPushButton *m_userIcon;
    QPushButton *m_accountIcon;
    QPushButton *m_passwdIcon;
    QLineEdit *m_userLineEdit;
    QLineEdit *m_accountLineEdit;
    QLineEdit *m_passwdLineEdit;
    QPushButton *m_registerButton;
    QTcpSocket *m_tcpSocket;

    bool m_isConnected;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
private slots:
    void m_register();
    void m_setConnected();
    void m_readReply();
};

#endif // REGISTERDIALOG_H
