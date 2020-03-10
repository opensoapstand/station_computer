TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x -pthread

LIBS += -pthread

SOURCES += \
        Hal.cpp \
        Marshall.cpp \
        Peripheral.cpp \
        Utils.cpp \
        main.cpp \
        stdafx.cpp

HEADERS += \
    Hal.h \
    Marshall.h \
    MarshallApi.h \
    Peripheral.h \
    Status.h \
    Utils.h \
    stdafx.h
