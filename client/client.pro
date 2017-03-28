#-------------------------------------------------
#
# Project created by QtCreator 2017-01-24T18:01:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network multimedia

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    predialog/predialog.cpp \
    predialog/registerdialog/registerdialog.cpp \
    messagewidget/messagestackedwidget.cpp \
    messagewidget/messagewidget.cpp \
    messagewidget/statewidget/statewidget.cpp \
    messagewidget/historywidget/historywidget.cpp \
    messagewidget/chatwidget/chatwidget.cpp \
    friendwidget/friendlistwidget.cpp \
    friendwidget/localwidget.cpp \
    friendwidget/friendButton/friendbutton.cpp \
    friendwidget/friendwidget.cpp \
    messagewidget/startwidget/startwidget.cpp \
    messagewidget/MessageBlockWidget/abstractmessagewidget.cpp \
    messagewidget/MessageBlockWidget/normalmessagewidget.cpp \
    messagewidget/chatwidget/emojiwidget.cpp \
    messagewidget/MessageBlockWidget/filemessagewidget/fileclient.cpp \
    messagewidget/MessageBlockWidget/filemessagereceivedwidget.cpp \
    messagewidget/MessageBlockWidget/filemessagesentwidget.cpp \
    messagewidget/MessageBlockWidget/imagemessagesentwidget.cpp \
    messagewidget/MessageBlockWidget/imagemessagewidget/imageprogresswidget.cpp \
    messagewidget/MessageBlockWidget/imagemessagereceivewidget.cpp

HEADERS  += widget.h \
    predialog/predialog.h \
    message/messagetype.h \
    predialog/registerdialog/registerdialog.h \
    messagewidget/messagestackedwidget.h \
    messagewidget/messagewidget.h \
    messagewidget/statewidget/statewidget.h \
    messagewidget/historywidget/historywidget.h \
    messagewidget/chatwidget/chatwidget.h \
    friendwidget/friendlistwidget.h \
    friendwidget/localwidget.h \
    friendwidget/friendButton/friendbutton.h \
    friendwidget/friendwidget.h \
    messagewidget/startwidget/startwidget.h \
    messagewidget/MessageBlockWidget/abstractmessagewidget.h \
    messagewidget/MessageBlockWidget/normalmessagewidget.h \
    messagewidget/chatwidget/emojiwidget.h \
    messagewidget/MessageBlockWidget/filemessagewidget/fileclient.h \
    messagewidget/MessageBlockWidget/filemessagereceivedwidget.h \
    messagewidget/MessageBlockWidget/filemessagesentwidget.h \
    messagewidget/MessageBlockWidget/imagemessagesentwidget.h \
    messagewidget/MessageBlockWidget/imagemessagewidget/imageprogresswidget.h \
    messagewidget/MessageBlockWidget/imagemessagereceivewidget.h

FORMS += \
    form.ui

RESOURCES += \
    res.qrc
