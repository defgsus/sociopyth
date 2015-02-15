#-------------------------------------------------
#
# Project created by QtCreator 2015-02-15T00:20:37
#
#-------------------------------------------------

QT       += core gui xml
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bioviewer_qt
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    biodata.cpp \
    biolistmodel.cpp \
    detailview.cpp \
    filtermodel.cpp

HEADERS  += mainwindow.h \
    biodata.h \
    biolistmodel.h \
    detailview.h \
    filtermodel.h
