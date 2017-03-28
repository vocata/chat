#include "chatwidget.h"
#include "emojiwidget.h"
#include "message/messagetype.h"

#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QEvent>
#include <QCloseEvent>
#include <QDateTime>
#include <QDebug>
#include <QJsonDocument>
#include <QMenu>
#include <QMessageBox>
#include <QWidgetAction>
#include <QRegularExpression>
#include <QFileDialog>

ChatWidget::ChatWidget(QWidget *parent) : QGroupBox(parent)
{
    m_chatTextEdit = new QTextEdit(this);
    m_emojiIcon = new QPushButton(this);
    m_photoIcon = new QPushButton(this);
    m_fileIcon = new QPushButton(this);
    m_sendButton = new QPushButton(this);
    m_emojiWidget = new EmojiWidget(this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
}

void ChatWidget::setAccount(const QString &localAccount, const QString &peerAccount)
{
    m_localAccount = localAccount;
    m_peerAccount = peerAccount;
}

bool ChatWidget::eventFilter(QObject *watched, QEvent *event) {
    if(watched == m_chatTextEdit) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Return && (keyEvent->modifiers() & Qt::ControlModifier)) {
               this->m_sendNolmalMessage();
               return true;
            }
        }
        return false;
    }
    return ChatWidget::eventFilter(watched, event);
}


void ChatWidget::m_setAttribution()
{
    QFont font;
    font.setPointSize(12);
    m_chatTextEdit->setFont(font);
    m_chatTextEdit->setFixedHeight(90);
    m_chatTextEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    m_chatTextEdit->setAcceptRichText(true);
    m_chatTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_chatTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_chatTextEdit->installEventFilter(this);
    m_chatTextEdit->setObjectName("chatLineEdit");

    QMenu *menu = new QMenu(this);
    QWidgetAction *widgetAction = new QWidgetAction(menu);
    widgetAction->setDefaultWidget(m_emojiWidget);
    menu->addAction(widgetAction);
    m_emojiIcon->setMenu(menu);
    m_emojiIcon->setStyleSheet("QPushButton::menu-indicator{image:none;}");
    m_emojiIcon->setToolTip(QStringLiteral("表情"));
    m_emojiIcon->setFixedSize(25, 25);
    m_emojiIcon->setIcon(QIcon(":/chatwidget/emoji"));
    m_emojiIcon->setObjectName("emojiIcon");

    m_photoIcon->setToolTip(QStringLiteral("图片"));
    m_photoIcon->setFixedSize(25, 25);
    m_photoIcon->setIcon(QIcon(":/chatwidget/photo"));
    m_photoIcon->setObjectName("pictureIcon");

    m_fileIcon->setToolTip(QStringLiteral("文件"));
    m_fileIcon->setFixedSize(25, 25);
    m_fileIcon->setIcon(QIcon(":/chatwidget/file"));
    m_fileIcon->setObjectName("fileIcon");

    m_sendButton->setText(QStringLiteral("发送(&s)"));
    m_sendButton->setFixedSize(70, 30);
    m_sendButton->setObjectName("sendButton");
}

void ChatWidget::m_setLayout()
{
      QHBoxLayout *iconBox = new QHBoxLayout();
      iconBox->addWidget(m_emojiIcon, 0, Qt::AlignRight);
      iconBox->addWidget(m_photoIcon);
      iconBox->addWidget(m_fileIcon);
      iconBox->setSpacing(10);

      QHBoxLayout *bottomBox = new QHBoxLayout();
      bottomBox->addLayout(iconBox);
      bottomBox->addWidget(m_sendButton, 0, Qt::AlignRight);
      bottomBox->setAlignment(iconBox, Qt::AlignLeft);
      bottomBox->setContentsMargins(10, 0, 10, 10);

      QVBoxLayout *box = new QVBoxLayout(this);
      box->addWidget(m_chatTextEdit);
      box->addLayout(bottomBox);
      box->setSpacing(0);
      box->setMargin(0);
}

void ChatWidget::m_setSignalSlot()
{
    connect(m_emojiWidget, &EmojiWidget::emojiClicked, this, &ChatWidget::m_insertEmoji);
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::m_sendNolmalMessage);
    connect(m_photoIcon, &QPushButton::clicked, this, &ChatWidget::m_sendImageMessage);
    connect(m_fileIcon, &QPushButton::clicked, this, &ChatWidget::m_sendFileMessage);
}

void ChatWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/chatwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void ChatWidget::m_insertEmoji(const QString &path)
{
    m_chatTextEdit->insertHtml(QString("<img src=\"%1\" />").arg(path));
}

void ChatWidget::m_sendNolmalMessage()
{
    if(m_chatTextEdit->document()->isEmpty())
        return;
    if(!m_online) {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("当前用户处于离线状态"));
        return;
    }

    QString html = m_chatTextEdit->toHtml();
    m_chatTextEdit->clear();
    QRegularExpression reg("<body[^>]*>([\\s\\S]*)<\\/body>");
    QRegularExpressionMatch match = reg.match(html);

    QString body = match.captured();
    QRegularExpression reg2("<(?!img|br|p|/p).*?>");
    QString content = body.replace(reg2, "");

    QVariantMap map;
    map.insert("peerAccount", m_peerAccount);
    map.insert("localAccount", m_localAccount);
    map.insert("messageId", QDateTime::currentMSecsSinceEpoch());
    map.insert("message", content);
    QJsonDocument doc = QJsonDocument::fromVariant(map);

    emit sendMessage(Message, doc);
}

void ChatWidget::m_sendImageMessage()
{
    if(!m_online) {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("当前用户处于离线状态"));
        return;
    }

    QString imageName = QFileDialog::getOpenFileName(this, QStringLiteral("选择图片"), "/home",
                                                     "Images (*.bmp *.jpeg *.jpg *.gif *.png)");

    if(QPixmap(imageName).isNull()) {
        return;
    }

    QVariantMap map;
    map.insert("peerAccount", m_peerAccount);
    map.insert("localAccount", m_localAccount);
    map.insert("messageId", QDateTime::currentMSecsSinceEpoch());
    map.insert("message", imageName);
    QJsonDocument doc = QJsonDocument::fromVariant(map);
    emit sendMessage(Image, doc);
}

void ChatWidget::m_sendFileMessage()
{
    if(!m_online) {
        QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("当前用户处于离线状态"));
        return;
    }

    QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("选择文件"), "/home", "Files (*.*)");
    if(fileName.isEmpty())
        return;

    QVariantMap map;
    map.insert("peerAccount", m_peerAccount);
    map.insert("localAccount", m_localAccount);
    map.insert("messageId", QDateTime::currentMSecsSinceEpoch());
    map.insert("message", fileName);
    QJsonDocument doc = QJsonDocument::fromVariant(map);
    emit sendMessage(File, doc);
}

