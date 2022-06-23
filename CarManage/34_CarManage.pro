#-------------------------------------------------
#
# Project created by QtCreator 2022-03-19T04:27:49
#
#-------------------------------------------------

QT       += core gui sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 34_CarManage
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    domxml.cpp \
    histogram.cpp \
    pie.cpp

HEADERS  += mainwindow.h \
    domxml.h \
    histogram.h \
    pie.h

FORMS    += mainwindow.ui
