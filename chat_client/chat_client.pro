#-------------------------------------------------
#
# Project created by QtCreator 2024-04-24T09:31:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chat_client
TEMPLATE = app
DEFINES += CHAT_CLIENT

SOURCES += main.cpp\
        mainwindow.cpp \
        myhighlighter.cpp \
    chatserver.cpp \
    logindialog.cpp

HEADERS  += mainwindow.h \
        myhighlighter.h \
    chatserver.h \
    logindialog.h

FORMS    += mainwindow.ui
