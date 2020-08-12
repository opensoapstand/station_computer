#-------------------------------------------------
#
# Project created by QtCreator 2019-10-30T09:55:05
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = simple_ui_setup

CONFIG += c++11

SOURCES += \
        database_logger.cpp \
        main.cpp \
        drinkfill_ev.cpp

HEADERS += \
        database_logger.h \
        drinkfill_ev.h

FORMS += \
        drinkfill_ev.ui

RESOURCES += \
    assets.qrc

