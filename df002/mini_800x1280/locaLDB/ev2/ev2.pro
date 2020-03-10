#-------------------------------------------------
#
# Project created by QtCreator 2019-07-15T10:37:04
#
#-------------------------------------------------

QT       += core gui virtualkeyboard sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EV2_Basic
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += \
    c++11 #\
    #console

CONFIG -= console

SOURCES += \
    beveragedata.cpp \
    database_logger.cpp \
    dispensewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    countdownwindow.cpp \
    posm/lrcgenerator.cpp \
    posm/packetreadsort.cpp \
    posm/mcommunication.cpp \
    posm/transactioninfo.cpp \
    #posm/packetfromecr.cpp \
    #posm/packetfromux410.cpp \
    #posm/transactionPackets.cpp \
    ardcommunication.cpp \
    payment.cpp \
    survey.cpp

HEADERS += \
    beveragedata.h \
    database_logger.h \
    dispensewindow.h \
    includefiles.h \
    mainwindow.h \
    countdownwindow.h \
    posm/enums.h \
    posm/lrcgenerator.h \
    posm/mcommunication.h \
    posm/packetfromecr.h \
    posm/packetfromux410.h \
    posm/packetreadsort.h \
    posm/transactionPackets.h \
    posm/transactioninfo.h \
    ardcommunication.h \
    payment.h \
    survey.h

FORMS += \
    mainwindow.ui \
    dispenseWindow.ui \
    countdownwindow.ui \
    payment.ui \
    survey.ui


#LIBS += -Lmoneris_obj/ -lmoneris
#OBJECTS += \
     #moneris_obj/moneriscommunication.o\
     #moneris_obj/packetfromecr.o\
     #moneris_obj/packetfromux410.o\
     #moneris_obj/transactionPackets.o\
     #moneris_obj/transactioninfo.o

RESOURCES += \
    assets.qrc

unix:!macx: LIBS += -L$$PWD/objm/ -lposm

INCLUDEPATH += $$PWD/objm
DEPENDPATH += $$PWD/objm

unix:!macx: PRE_TARGETDEPS += $$PWD/objm/libposm.a

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
