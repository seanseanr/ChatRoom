#-------------------------------------------------
#
# Project created by QtCreator 2024-04-24T09:31:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chatserver_server
TEMPLATE = app
#DEFINES += CHAT_CLIENT

SOURCES += ../chatserver/main.cpp\
        ../chatserver/mainwindow.cpp \
        ../chatserver/myhighlighter.cpp \
    ../chatserver/chatserver.cpp \
    ../chatserver/logindialog.cpp \
    ../chatserver/pictureserver.cpp

HEADERS  += ../chatserver/mainwindow.h \
        ../chatserver/myhighlighter.h \
    ../chatserver/chatserver.h \
    ../chatserver/logindialog.h \
    ../chatserver/pictureserver.h

FORMS    += ../chatserver/mainwindow.ui
