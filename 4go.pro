#-------------------------------------------------
#
# Project created by QtCreator 2017-01-09T16:48:13
#
#-------------------------------------------------

QT       += core gui
# QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 4go
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    object.cpp \
    parameter.cpp \
    def.cpp \
    drawchess.cpp \
    utils.cpp \
    drawboard.cpp \
    route.cpp \
    labor.cpp \
    strategy1.cpp \
    strategy2.cpp \
    strategy3.cpp \
    strategy1a.cpp \
    strategy2a.cpp

HEADERS  += \
    main.h \
    object.h \
    parameter.h \
    def.h \
    drawchess.h \
    utils.h \
    drawboard.h \
    route.h \
    labor.h \
    strategies.h
