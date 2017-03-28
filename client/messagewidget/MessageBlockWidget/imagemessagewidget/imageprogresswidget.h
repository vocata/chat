#ifndef IMAGEPROGRESSWIDGET_H
#define IMAGEPROGRESSWIDGET_H

#include <QLabel>
#include <QMovie>
#include <QWidget>

class QProgressBar;
class QMenu;
class ImageProgressWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ImageProgressWidget(QWidget *parent = 0);
    void setImage(const QString &imageName);
    void setAspectRatio(qint32 w, qint32 h);
    qint32 width();
    qint32 height();
public slots:
    void updateProgress(qint64 value, qint64 total);
protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);
private:
    QLabel *m_imageLabel;
    QProgressBar *m_progressBar;
    QMenu *m_rightMenu;
    QAction *m_saveAsAction;
    QAction *m_openDirAction;
    QMovie m_movie;
    QString m_imageName;
    QByteArray m_imageData;

    void m_setAttribution();
    void m_setLayout();
    void m_setSignalSlot();
    void m_setStyleSheet();
private slots:
    void m_saveAs();
    void m_openFile();
    void m_openDir();
};

inline qint32 ImageProgressWidget::width() { return m_imageLabel->width(); }
inline qint32 ImageProgressWidget::height() { return m_imageLabel->height(); }

#endif // IMAGEPROGRESSWIDGET_H
