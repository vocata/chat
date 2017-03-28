#ifndef EMOJIWIDGET_H
#define EMOJIWIDGET_H

#include <QWidget>

class QButtonGroup;
class QAbstractButton;

class EmojiWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EmojiWidget(QWidget *parent = 0);
signals:
    void emojiClicked(const QString &path);
private:
    QButtonGroup *m_buttonGroup;
    void m_setStyleSheet();
private slots:
    void m_buttonClick(QAbstractButton *button);
};

#endif // EMOJIWIDGET_H
