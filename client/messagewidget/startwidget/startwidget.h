#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QGroupBox>

class QLabel;

class StartWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit StartWidget(QWidget *parent = 0);
private:
    QLabel *m_startLabel;
    void m_setStyleSheet();
};

#endif // STARTWIDGET_H
