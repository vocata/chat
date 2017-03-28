#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtSql>

#include <QDebug>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("test");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("19950716");
    if (!db.open()) {
        qDebug() << "Unable to establish a database connection.\n";
        return false;
    }

    /* query operation, such as creating tables */
    QSqlQuery query;

    QStringList tableList = QSqlDatabase::database().tables();
    qDebug() << tableList;
    if(!tableList.contains(QString("user_info"))) {
        query.exec(QString("create table user_info(account VARCHAR(11), passwd VARCHAR(16), user VARCHAR(20),"
                   "primary key(account))"));
    }
    if(!tableList.contains(QString("user_message"))) {
        query.exec(QString("create table user_message(peerAccount VARCHAR(11), type INT UNSIGNED, messageId BIGINT UNSIGNED, message TEXT,"
                   "primary key(peerAccount, messageId))"));
    }
    return true;
}

static void closeConnection()
{
    QSqlDatabase::database().close();
}

#endif // CONNECTION_H

