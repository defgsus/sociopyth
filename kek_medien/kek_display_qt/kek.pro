#-------------------------------------------------
#
# Project created by QtCreator 2015-02-05T11:40:43
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = kek
TEMPLATE = app

CONFIG += c++11

INCLUDEPATH += /home/defgsus/prog/C/lib/dlib-18.13
LIBS += -lX11

SOURCES += main.cpp\
        mainwindow.cpp \
    springsystem.cpp \
    kekscene.cpp \
    kekdata.cpp \
    kekmodel.cpp \
    companyview.cpp \
    kekview.cpp \
    kekdata_dlib.cpp \
    /home/defgsus/prog/C/lib/dlib-18.13/dlib/all/source.cpp

HEADERS  += mainwindow.h \
    springsystem.h \
    kekscene.h \
    kekdata.h \
    kekmodel.h \
    companyview.h \
    kekview.h
