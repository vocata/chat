#include "predialog.h"
#include "message/messagetype.h"
#include "predialog/registerdialog/registerdialog.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTcpSocket>

#include <QDebug>

PreDialog::PreDialog(QTcpSocket *socket, QWidget *parent) : QDialog(parent)
{
    m_accountIcon = new QPushButton(this);
    m_passwdIcon = new QPushButton(this);
    m_accountLineEdit = new QLineEdit(this);
    m_passwdLineEdit = new QLineEdit(this);
    m_loginButton = new QPushButton(this);
    m_registerButton = new QPushButton(this);
    m_findPasswdButton = new QPushButton(this);
    m_tcpSocket = socket;
    m_isLogined = false;

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();

    this->setFixedSize(Width, Height);
    qint32 index = QApplication::desktop()->primaryScreen();
    qint32 desktopWidget = QApplication::desktop()->screen(index)->width();
    qint32 desktopHeight = QApplication::desktop()->screen(index)->height();
    this->move((desktopWidget - Width)/2, (desktopHeight - Height)/2);     //窗口居中

    this->setWindowTitle(QStringLiteral("登录"));
}

QMap<QString, QString> PreDialog::getLoginedInfo()
{
    QMap<QString, QString> info;

    if(m_isLogined) {
        info.insert("account", m_account);
        info.insert("passwd", m_passwd);
        info.insert("user", m_user);
        return info;
    }
    else {
        return info;
    }
}

void PreDialog::m_setAttribution()
{
    m_accountIcon->setIcon(QIcon(":/predialog/account"));
    m_accountIcon->setFixedSize(40, 40);
    m_accountIcon->setFocusPolicy(Qt::NoFocus);
    m_accountIcon->setObjectName("accountIcon");
    m_passwdIcon->setIcon(QIcon(":/predialog/passwd"));
    m_passwdIcon->setFixedSize(40, 40);
    m_passwdIcon->setFocusPolicy(Qt::NoFocus);
    m_passwdIcon->setObjectName("passwdIcon");

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

    m_loginButton->setCursor(Qt::PointingHandCursor);
    m_loginButton->setText(QStringLiteral("立即登录"));
    m_loginButton->setFixedSize(250, 40);
    m_loginButton->setObjectName("loginButton");

    m_registerButton->setCursor(Qt::PointingHandCursor);
    m_registerButton->setText(QStringLiteral("注册"));
    m_registerButton->setObjectName("registerButton");
    m_findPasswdButton->setCursor(Qt::PointingHandCursor);
    m_findPasswdButton->setText(QStringLiteral("找回密码"));
    m_findPasswdButton->setObjectName("findPasswdButton");
}

void PreDialog::m_setLayout()
{
    /* top */
    QHBoxLayout *accountIconLineEditBox = new QHBoxLayout();
    accountIconLineEditBox->addWidget(m_accountIcon);
    accountIconLineEditBox->addWidget(m_accountLineEdit);
    accountIconLineEditBox->setSpacing(0);
    QHBoxLayout *passwdIconLineEditBox = new QHBoxLayout();
    passwdIconLineEditBox->addWidget(m_passwdIcon);
    passwdIconLineEditBox->addWidget(m_passwdLineEdit);
    passwdIconLineEditBox->setSpacing(0);

    QVBoxLayout *iconLineEditBox = new QVBoxLayout();
    iconLineEditBox->addLayout(accountIconLineEditBox);
    iconLineEditBox->addLayout(passwdIconLineEditBox);
    iconLineEditBox->setSpacing(0);                    //the spacing between lineEdit

    /* bottom */
    QHBoxLayout *bottomButtonBox = new QHBoxLayout();
    bottomButtonBox->addWidget(m_registerButton, 0, Qt::AlignLeft);
    bottomButtonBox->addWidget(m_findPasswdButton, 0, Qt::AlignRight);
    bottomButtonBox->setSpacing(0);

    QVBoxLayout *buttonBox = new QVBoxLayout();
    buttonBox->addWidget(m_loginButton);
    buttonBox->addLayout(bottomButtonBox);
    buttonBox->setSpacing(10);                          //the spacing between botton

    /* all */
    QVBoxLayout *box = new QVBoxLayout(this);
    box->addLayout(iconLineEditBox);
    box->addLayout(buttonBox);
    box->setAlignment(iconLineEditBox, Qt::AlignHCenter | Qt::AlignBottom);
    box->setAlignment(buttonBox, Qt::AlignHCenter | Qt::AlignTop);
    box->setSpacing(30);                                //the spacing between lineEdit and botton
}

void PreDialog::m_setSignalSlot()
{
    connect(m_loginButton, &QPushButton::clicked, this, &PreDialog::m_login);
    connect(m_registerButton, &QPushButton::clicked, this, &PreDialog::m_register);
    connect(m_tcpSocket, &QTcpSocket::connected, this, &PreDialog::m_sendRequest);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &PreDialog::m_readReply);
    connect(m_tcpSocket, static_cast<void (QAbstractSocket:: *)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &PreDialog::m_errorHandler);

}

void PreDialog::m_setStyleSheet()
{
    QFile qssFile(":/qss/predialog");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void PreDialog::m_setLineEditEnable(bool enable)
{
    m_accountLineEdit->setEnabled(enable);
    m_passwdLineEdit->setEnabled(enable);
}

void PreDialog::closeEvent(QCloseEvent *event)
{
    if(m_isLogined)
        this->setResult(QDialog::Accepted);
    else
        this->setResult(QDialog::Rejected);

    event->accept();
}

void PreDialog::m_login()
{
//    m_accountLineEdit->setText("13715661201");
//    m_passwdLineEdit->setText("666666");

    m_account = m_accountLineEdit->text().trimmed().replace(" ", "");
    m_passwd = m_passwdLineEdit->text().trimmed().replace(" ", "");;
    qDebug() << m_account << m_passwd;
    if(!QRegExp("1[\\d]{10}").exactMatch(m_account)) {
        QMessageBox::information(this, QStringLiteral("登录错误"), QStringLiteral("请输入11位手机号码!"));
        return;
    }
    if(m_passwd.size() < PasswdMinSize) {
        QMessageBox::information(this, QStringLiteral("登录错误"), QStringLiteral("请输入6-20位的密码!"));
        return;
    }

    QDataStream out(&m_request, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_4);
    out << static_cast<quint32>(0) << static_cast<quint32>(Login) << m_account << m_passwd;
    quint32 size = m_request.size();
    out.device()->seek(0);
    out << size;

    this->m_setLineEditEnable(false);
    m_tcpSocket->connectToHost(REMOTEHOST_ADDRESS, REMOTEHOST_CHAT_PORT);
}

void PreDialog::m_register()
{
    RegisterDialog dialog(this);
    dialog.exec();
}

void PreDialog::m_sendRequest()
{
    QByteArray request = m_request;
    m_tcpSocket->write(request);
    m_request.clear();
}

void PreDialog::m_readReply()
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

    if(type != Login)
        return;

    if(bytesReceived < totalBytes) {
        QByteArray temp;
        temp = m_tcpSocket->read(totalBytes - bytesReceived);
        fullData.append(temp);
        bytesReceived += temp.size();
    }

    if(bytesReceived == totalBytes) {
        QDataStream reply(&fullData, QIODevice::ReadOnly);
        quint32 result;
        reply >> result;
        if(result == LoginSuccess) {
            reply >> m_user;
            m_isLogined = true;

            disconnect(m_tcpSocket, &QTcpSocket::connected, this, &PreDialog::m_sendRequest);
            disconnect(m_tcpSocket, &QTcpSocket::readyRead, this, &PreDialog::m_readReply);
            disconnect(m_tcpSocket, static_cast<void (QAbstractSocket:: *)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, &PreDialog::m_errorHandler);

            this->close();
        } else if(result == PasswdError) {
            m_isLogined = false;
            QMessageBox::information(this, QStringLiteral("登录失败"), QStringLiteral("账号与密码不匹配!"));
            m_tcpSocket->disconnectFromHost();
            this->m_setLineEditEnable(true);
        } else {
            m_isLogined = false;
            QMessageBox::information(this, QStringLiteral("登录失败"), QStringLiteral("该账号不存在!"));
            m_tcpSocket->disconnectFromHost();
            this->m_setLineEditEnable(true);
        }

        fullData.clear();
        bytesReceived = 0;
        totalBytes = 0;
    }
}

void PreDialog::m_errorHandler(QAbstractSocket::SocketError error)
{
    switch(error) {
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, QStringLiteral("服务器错误"), QStringLiteral("服务器维护中!"));
        break;
    case QAbstractSocket::RemoteHostClosedError:
        QMessageBox::information(this, QStringLiteral("服务器错误"), QStringLiteral("服务器维护中!"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::information(this, QStringLiteral("无网络"), QStringLiteral("网络未连接!"));
        break;
    default:
        break;
    }
    this->m_setLineEditEnable(true);
    qDebug() << error;
}

