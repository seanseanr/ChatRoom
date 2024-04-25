#-------------------------------------------------
#
# Project created by QtCreator 2024-04-24T09:31:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chatserver
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        myhighlighter.cpp \
    chatserver.cpp

HEADERS  += mainwindow.h \
        myhighlighter.h \
    chatserver.h

FORMS    += mainwindow.ui
