#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H

#include <qglobal.h>

enum TcpMsgType: quint32 { Login = 0, Logout, Register, GetList, Message, Image, File, ACK, GetRestMessage, HeartBeat };

const qint32 UserMinSize = 2;
const qint32 UserMaxSize = 8;

const qint32 PasswdMinSize = 6;
const qint32 PasswdMaxSize = 16;

const qint32 AccountSize = 11;

#define REMOTEHOST_ADDRESS      ("120.77.156.128")
#define REMOTEHOST_CHAT_PORT    (7788)
#define REMOTEHOST_FILE_PORT    (7789)

#endif // MESSAGETYPE_H

