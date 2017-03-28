#include "emojiwidget.h"

#include <QButtonGroup>
#include <QDir>
#include <QGridLayout>
#include <QPushButton>

#include <QDebug>

EmojiWidget::EmojiWidget(QWidget *parent) : QWidget(parent)
{
    m_buttonGroup = new QButtonGroup(this);

    QDir dir(":/emoji");
    QStringList emojiList = dir.entryList(QDir::Files);

    QGridLayout *box = new QGridLayout(this);
    const quint32 maxColumn = 8;
    quint32 row = 0, column = 0;
    for(const auto &emoji: emojiList) {
        QPushButton *button = new QPushButton(this);
        button->setCursor(Qt::PointingHandCursor);
        QString emojiPath = QString(":/emoji/%1").arg(emoji);
        button->setIcon(QIcon(emojiPath));
        button->setIconSize(QSize(25, 25));
        button->setFixedSize(QSize(25, 25));
        button->setObjectName(emojiPath);
        m_buttonGroup->addButton(button);

        box->addWidget(button, row, column, 1, 1);
        column++;
        if(column == maxColumn) {
            column = 0;
            row++;
        }
    }
    box->setSpacing(10);

    connect(m_buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(m_buttonClick(QAbstractButton*)));

    this->m_setStyleSheet();
}

void EmojiWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/emojiwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void EmojiWidget::m_buttonClick(QAbstractButton *button)
{
    emit emojiClicked(button->objectName());
    this->parentWidget()->close();
}

