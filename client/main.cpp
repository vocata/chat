#include "widget.h"
#include <QApplication>
#include <QTextCodec>
#include <QFontDatabase>
#include <QDebug>

#define EXIT_RESTART 774;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));       //设置编码格式

#ifdef _WIN32
    QFont font("Microsoft Yahei", 10);
#else
    QFont font("DejaVuSans", 10);
#endif
    QApplication::setFont(font);

    int ret = -1;
    do {
        Widget w;
        if(!w.startChat()) {
            ret = 0;
            break;
        }
        ret = a.exec();
    } while(ret != 0);

    return ret;
}
