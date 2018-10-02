#-------------------------------------------------
#
# Project created by QtCreator 2015-07-27T16:38:11
#
#-------------------------------------------------

QT += core gui
QT += network
QT += webkit webkitwidgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = rename
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    rjutility.cpp

HEADERS  += mainwindow.h \
    rjutility.h

FORMS    += mainwindow.ui
