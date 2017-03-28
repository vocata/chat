#include "imageprogresswidget.h"

#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QFileDialog>
#include <QDesktopServices>

#include <QDebug>

ImageProgressWidget::ImageProgressWidget(QWidget *parent) : QWidget(parent)
{
    m_imageLabel = new QLabel(this);
    m_progressBar = new QProgressBar(this);
    m_rightMenu = new QMenu(this);
    m_saveAsAction = new QAction(this);
    m_openDirAction = new QAction(this);

    this->m_setAttribution();
    this->m_setLayout();
    this->m_setSignalSlot();
    this->m_setStyleSheet();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
}

void ImageProgressWidget::setImage(const QString &imageName)
{
    m_imageName = imageName;
    QFile image(imageName);
    if(!image.open(QIODevice::ReadOnly)) {
        qDebug() << "image open error";
        return;
    }
    m_imageData = image.readAll();

    m_movie.setFileName(m_imageName);
    m_imageLabel->setMovie(&m_movie);
    m_movie.start();

    QPixmap pix(m_imageName);
    if(pix.width() > 350)
        m_imageLabel->setFixedWidth(350);
    else
        m_imageLabel->setFixedWidth(pix.width());
    double scale = (double)pix.width() / m_imageLabel->width();
    m_imageLabel->setFixedHeight((double)pix.height() / scale);
    m_progressBar->setFixedSize(m_imageLabel->width(), 5);

    m_imageLabel->installEventFilter(this);         //只有设置了图片才可以双击打开
}

void ImageProgressWidget::setAspectRatio(qint32 w, qint32 h)
{
    m_imageLabel->setFixedSize(w, h);
    m_progressBar->setFixedSize(w, 5);
}

void ImageProgressWidget::updateProgress(qint64 value, qint64 total)
{
    m_progressBar->setRange(0, total);
    m_progressBar->setValue(value);
    if(total == value) {
        m_progressBar->hide();
    }
}

bool ImageProgressWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == m_imageLabel) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (mouseEvent->button() == Qt::LeftButton) {
               this->m_openFile();
               return true;
            }
        }
        return false;
    }
    return ImageProgressWidget::eventFilter(watched, event);
}

void ImageProgressWidget::contextMenuEvent(QContextMenuEvent *event)
{
    m_rightMenu->clear();
    m_rightMenu->addAction(m_saveAsAction);
    m_rightMenu->addAction(m_openDirAction);

    m_rightMenu->exec(QCursor::pos());
    event->accept();
}

void ImageProgressWidget::m_setAttribution()
{
    m_imageLabel->setCursor(Qt::PointingHandCursor);
    m_imageLabel->setScaledContents(true);
    m_imageLabel->setObjectName("imageLabel");

    m_progressBar->setFormat("");
    m_progressBar->setValue(0);
    m_progressBar->setObjectName("progressBar");

//    m_rightMenu->setFixedWidth(100);
    m_rightMenu->setObjectName("rightMenu");

    m_saveAsAction->setText(QStringLiteral("另存为"));
    m_saveAsAction->setObjectName("saveAsAction");
    m_openDirAction->setText(QStringLiteral("打开文件夹"));
    m_openDirAction->setObjectName("openDirAction");
}

void ImageProgressWidget::m_setLayout()
{
    QVBoxLayout *box = new QVBoxLayout(this);
    box->addWidget(m_imageLabel);
    box->addWidget(m_progressBar);
    box->setSpacing(5);
    box->setMargin(0);
}

void ImageProgressWidget::m_setSignalSlot()
{
    connect(m_saveAsAction, &QAction::triggered, this, &ImageProgressWidget::m_saveAs);
    connect(m_openDirAction, &QAction::triggered, this, &ImageProgressWidget::m_openDir);
}

void ImageProgressWidget::m_setStyleSheet()
{
    QFile qssFile(":/qss/imageprogresswidget");
    qssFile.open(QFile::ReadOnly);
    QString styleSheet = qssFile.readAll();
    this->setStyleSheet(styleSheet);
}

void ImageProgressWidget::m_saveAs()
{
    QString fileName = QFileInfo(m_imageName).fileName();
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存文件"),
                                                    QString("/home/%1").arg(fileName), "Files (*.*)");
    if(filePath.isEmpty())
        return;

    QFile image(filePath);
    if(!image.open(QIODevice::WriteOnly)) {
        qDebug() << "image open error";
        return;
    }
    image.write(m_imageData);
    image.flush();
}

void ImageProgressWidget::m_openFile()
{
    QUrl url = QUrl::fromLocalFile(m_imageName);
    QDesktopServices::openUrl(url);
}

void ImageProgressWidget::m_openDir()
{
    QUrl url = QUrl::fromLocalFile(QFileInfo(m_imageName).absolutePath());
    QDesktopServices::openUrl(url);
}
