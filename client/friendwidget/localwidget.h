#ifndef LOCALWIDGET_H
#define LOCALWIDGET_H

#include <QGroupBox>
#include <QMap>

class QLabel;
class QPushButton;
class QMenu;
class LocalWidget : public QGroupBox
{
    Q_OBJECT
public:
    explicit LocalWidget(QWidget *parent = 0);
    void setLoginedInfo(const QMap<QString, QString> &info);
    void setState(bool state);
signals:
    void offline();
    void online();
protected:
private:
    QLabel *m_userLabel;
    QPushButton *m_stateIcon;
    QMenu *m_stateMenu;
    QAction *m_onlineAction;
    QAction *m_offlineAction;
    QAction *m_restartAction;
    bool m_online;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
private slots:
    void m_setOnline();
    void m_setOffline();
    void m_setState(bool state);
    void m_restart();
};

#endif // LOCALWIDGET_H
