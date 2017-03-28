#include "registerdialog.h"
#include "message/messagetype.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTcpSocket>
#include <QVBoxLayout>
#include <QFile>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent)
{
    m_userIcon = new QPushButton(this);
    m_accountIcon = new QPushButton(this);
    m_passwdIcon = new QPushButton(this);
    m_userLineEdit = new QLineEdit(this);
    m_accountLineEdit = new QLineEdit(this);
    m_passwdLineEdit = new QLineEdit(this);
    m_registerButton = new QPushButton(this);
    m_tcpSocket = new QTcpSocket(this);
    m_isConnected = false;

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
    this->setWindowTitle(QStringLiteral("注册"));

    m_tcpSocket->connectToHost(REMOTEHOST_ADDRESS, REMOTEHOST_CHAT_PORT);
}

void RegisterDialog::m_setAttribution()
{
    m_userIcon->setIcon(QIcon(":/predialog/user"));
    m_userIcon->setFixedSize(40, 40);
    m_userIcon->setFocusPolicy(Qt::NoFocus);
    m_userIcon->setObjectName("userIcon");
    m_accountIcon->setIcon(QIcon(":/predialog/account"));
    m_accountIcon->setFixedSize(40, 40);
    m_accountIcon->setFocusPolicy(Qt::NoFocus);
    m_accountIcon->setObjectName("accountIcon");
    m_passwdIcon->setIcon(QIcon(":/predialog/passwd"));
    m_passwdIcon->setFixedSize(40, 40);
    m_passwdIcon->setFocusPolicy(Qt::NoFocus);
    m_passwdIcon->setObjectName("passwdIcon");

    m_userLineEdit->setPlaceholderText(QStringLiteral("请输入用户名"));
    m_userLineEdit->setMaxLength(UserMaxSize);
    m_userLineEdit->setFixedSize(210, 40);
    m_userLineEdit->setObjectName("userLineEdit");
    m_accountLineEdit->setPlaceholderText(QStringLiteral("请输入手机号"));
    m_accountLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_accountLineEdit->setMaxLength(AccountSize);
    m_accountLineEdit->setFixedSize(210, 40);
    m_accountLineEdit->setObjectName("accountLineEdit");
    m_passwdLineEdit->setPlaceholderText(QStringLiteral("请输入密码"));
    m_passwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_passwdLineEdit->setMaxLength(PasswdMaxSize);
    m_passwdLineEdit->setEchoMode(QLineEdit::Password);
    m_passwdLineEdit->setFixedSize(210, 40);
    m_passwdLineEdit->setObjectName("passwdLineEdit");

    m_registerButton->setCursor(Qt::PointingHandCursor);
    m_registerButton->setText(QStringLiteral("立即注册"));
    m_registerButton->setFixedSize(250, 40);
    m_registerButton->setObjectName("registerButton");
}

void RegisterDialog::m_setLayout()
{
    /* top */
    QHBoxLayout *userIconLineEditBox = new QHBoxLayout();
    userIconLineEditBox->addWidget(m_userIcon);
    userIconLineEditBox->addWidget(m_userLineEdit);
    userIconLineEditBox->setSpacing(0);
    QHBoxLayout *accountIconLineEditBox = new QHBoxLayout();
    accountIconLineEditBox->addWidget(m_accountIcon);
    accountIconLineEditBox->addWidget(m_accountLineEdit);
    accountIconLineEditBox->setSpacing(0);
    QHBoxLayout *passwdIconLineEditBox = new QHBoxLayout();
    passwdIconLineEditBox->addWidget(m_passwdIcon);
    passwdIconLineEditBox->addWidget(m_passwdLineEdit);
    passwdIconLineEditBox->setSpacing(0);

    QVBoxLayout *iconLineEditBox = new QVBoxLayout();
    iconLineEditBox->addLayout(userIconLineEditBox);
    iconLineEditBox->addLayout(accountIconLineEditBox);
    iconLineEditBox->addLayout(passwdIconLineEditBox);
    iconLineEditBox->setSpacing(0);                    //the spacing between lineEdit

    /* all */
    QVBoxLayout *box = new QVBoxLayout(this);
    box->addLayout(iconLineEditBox);
    box->addWidget(m_registerButton);
    box->setAlignment(iconLineEditBox, Qt::AlignHCenter | Qt::AlignBottom);
    box->setAlignment(m_registerButton, Qt::AlignHCenter | Qt::AlignTop);
    box->setSpacing(30);                                //the spacing between lineEdit and botton

    this->setFixedSize(350, 250);
}

void RegisterDialog::m_setSignalSlot()
{
    connect(m_registerButton, &QPushButton::clicked, this, &RegisterDialog::m_register);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &RegisterDialog::m_setConnected);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &RegisterDialog::m_readReply);
}

void RegisterDialog::m_setStyleSheet()
{
    QFile qssFile(":/qss/registerdialog");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void RegisterDialog::m_register()
{
    if(m_isConnected) {
        QString user = m_userLineEdit->text().trimmed().replace(" ", "");;
        QString account = m_accountLineEdit->text().trimmed().replace(" ", "");;
        QString passwd = m_passwdLineEdit->text().trimmed().replace(" ", "");;
        if(user.size() < UserMinSize) {
            QMessageBox::information(this, QStringLiteral("注册错误"), QStringLiteral("请输入2-20位的用户名!"));
            return;
        }
        if(!QRegExp("1[\\d]{10}").exactMatch(account)) {
            QMessageBox::information(this, QStringLiteral("注册错误"), QStringLiteral("请输入11位手机号码!"));
            return;
        }
        if(passwd.size() < PasswdMinSize) {
            QMessageBox::information(this, QStringLiteral("注册错误"), QStringLiteral("请输入6-20位的密码!"));
            return;
        }

        QByteArray request;
        QDataStream out(&request, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);
        out << static_cast<quint32>(0) << static_cast<quint32>(Register) << user << account << passwd;

        quint32 size = request.size();
        out.device()->seek(0);
        out << size;
        m_tcpSocket->write(request);
    } else {
        QMessageBox::information(this, QStringLiteral("无网络"), QStringLiteral("网络未连接!"));
    }

}

void RegisterDialog::m_setConnected()
{
    m_isConnected = true;
}

void RegisterDialog::m_readReply()
{
    static QByteArray fullData;
    static quint32 bytesReceived = 0, totalBytes = 0;
    quint32 type;

    QDataStream in(m_tcpSocket);
    in.setVersion(QDataStream::Qt_5_4);

    if(bytesReceived < 2 * sizeof(quint32)) {
        if(m_tcpSocket->bytesAvailable() >= (qint64)(2 * sizeof(quint32))) {
            in >> totalBytes >> type;
            bytesReceived += 2 * sizeof(quint32);
        } else {
            return;
        }
    }

    if(type != Register)
        qDebug() << "message error";

    if(bytesReceived < totalBytes) {
        QByteArray temp;
        temp = m_tcpSocket->read(totalBytes - bytesReceived);
        fullData.append(temp);
        bytesReceived += temp.size();
    }

    if(bytesReceived == totalBytes) {
        QDataStream reply(&fullData, QIODevice::ReadOnly);
        reply.setVersion(QDataStream::Qt_5_4);
        quint32 result;

        reply >> result;
        if(result == RegisterSuccess) {
            QMessageBox::information(this, QStringLiteral("注册成功"), QStringLiteral("恭喜您，注册成功!"));
            this->close();
        } else {
            QMessageBox::information(this, QStringLiteral("注册失败"), QStringLiteral("该账号已存在!"));
        }

        fullData.clear();
        bytesReceived = 0;
        totalBytes = 0;
    }
}
