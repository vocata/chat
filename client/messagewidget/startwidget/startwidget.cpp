#include "startwidget.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>

StartWidget::StartWidget(QWidget *parent) : QGroupBox(parent)
{
    m_startLabel = new QLabel(QStringLiteral("无会话"), this);

    QFont font;
    font.setPointSize(20);
    m_startLabel->setFont(font);
    m_startLabel->setObjectName("startLabel");

    QHBoxLayout *box = new QHBoxLayout(this);
    box->addWidget(m_startLabel, 0, Qt::AlignCenter);
    this->m_setStyleSheet();
}

void StartWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/startwidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

