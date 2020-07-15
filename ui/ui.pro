QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += \
    c++11

FORMS += \
    dispensepage.ui \
    idle.ui \
    paypage.ui \
    payselect.ui \
    productpage_1.ui \
    productpage_2.ui \
    thankyoupage.ui

HEADERS += \
    dispensepage.h \
    idle.h \
    includefiles.h \
    paypage.h \
    payselect.h \
    productpage_1.h \
    productpage_2.h \
    thankyoupage.h

SOURCES += \
    dispensepage.cpp \
    idle.cpp \
    main.cpp \
    paypage.cpp \
    payselect.cpp \
    productpage_1.cpp \
    productpage_2.cpp \
    thankyoupage.cpp

RESOURCES += \
    src.qrc
