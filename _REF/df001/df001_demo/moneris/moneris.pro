TEMPLATE = app
QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
        lrcgenerator.cpp \
        main.cpp \
        moneriscommunication.cpp \
        packetfromecr.cpp \
        packetfromux410.cpp \
        packetreadsort.cpp \
        paymentdatabase.cpp \
        simulator.cpp \
        transactionPackets.cpp \
        transactioninfo.cpp

HEADERS += \
    enums.h \
    lrcgenerator.h \
    moneriscommunication.h \
    packetfromecr.h \
    packetfromux410.h \
    packetreadsort.h \
    paymentdatabase.h \
    simulator.h \
    transactionPackets.h \
    transactioninfo.h

FORMS += \
    simulator.ui
