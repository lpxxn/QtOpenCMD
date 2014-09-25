#-------------------------------------------------
#
# Project created by QtCreator 2014-09-23T08:07:20
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app
CONFIG += c++11

LIBS += shell32.lib

SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui
