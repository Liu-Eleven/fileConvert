#-------------------------------------------------
#
# Project created by QtCreator 2017-02-06T22:37:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileConvert
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

DESTDIR = bin
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
UI_DIR = tmp/ui
RCC_DIR = tmp/rcc
