QT += core
QT -= gui
QT += network sql

TARGET = server
CONFIG += console
CONFIG -= app_bundle
CONFIG += c++14

TEMPLATE = app

SOURCES += main.cpp \
    server.cpp \
    tcp/tcpsocket.cpp

HEADERS += \
    server.h \
    sql/connection.h \
    tcp/tcpsocket.h

