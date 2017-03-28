#include "filehandler.h"
#include "fileserver.h"

#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <QTcpServer>
#include <QTcpSocket>

#include <QDebug>

typedef void Sigfunc(int);
Sigfunc *Signal(int signo, Sigfunc *func)
{
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    if(signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
#ifdef SA_RESTART
        act.sa_flags |= SA_RESTART;
#endif
    }
    if(sigaction(signo, &act, &oact) < 0)
        return SIG_ERR;
    return(oact.sa_handler);
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        qDebug() << QString("child %1 terminated when signal %2 occur").arg(pid).arg(signo);
    return;
}

FileServer::FileServer(QObject *parent) : QTcpServer(parent)
{
    Signal(SIGCHLD, sig_chld);
}

void FileServer::start()
{
    this->listen(QHostAddress::Any, FILE_SERVER_PORT);
}

void FileServer::incomingConnection(qintptr handle)
{
    pid_t pid;
    if( (pid = fork()) == 0) {
        ::close(this->socketDescriptor());

        FileHandler *fileHandler = new FileHandler(this);
        fileHandler->start(handle);
    } else {
        ::close(handle);
    }
}
