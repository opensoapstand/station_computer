QT += core gui sql widgets serialport network svg quick qml dbus

TARGET = DF_UI

CONFIG += \
    c++11

FORMS += \
    page_dispenser.ui \
    wifiErrorPage.ui \
    idle.ui \
    pagepayment.ui \
    page_product.ui \
    pageproductsoverview.ui \
    pagethankyou.ui \
    page_maintenance.ui \
    page_maintenance_dispenser.ui \
    init.ui \
    help.ui \
    keyboard.ui

HEADERS += \
    dfuicommthread.h \
    dfuiserver.h \
    page_dispenser.h \
    idle.h \
    includefiles.h \
    pagepayment.h \
    page_product.h \
    posm/enums.h \
    posm/lrcgenerator.h \
    posm/mcommunication.h \
    posm/packetfromecr.h \
    posm/packetfromux410.h \
    posm/packetreadsort.h \
    posm/paymentdatabase.h \
    posm/transactionPackets.h \
    posm/transactioninfo.h \
    pageproductsoverview.h \
    pagethankyou.h \
    df_util.h \
    drinkorder.h \
    dbmanager.h \
    page_maintenance.h \
    page_maintenance_dispenser.h \
    wifiErrorPage.h \
    init.h \
    help.h

SOURCES += \
    dfuicommthread.cpp \
    dfuiserver.cpp \
    page_dispenser.cpp \
    idle.cpp \
    main.cpp \
    pagepayment.cpp \
    page_product.cpp \
    posm/lrcgenerator.cpp \
    posm/mcommunication.cpp \
    posm/packetfromecr.cpp \
    posm/packetfromux410.cpp \
    posm/packetreadsort.cpp \
    posm/paymentdatabase.cpp \
    posm/transactionPackets.cpp \
    posm/transactioninfo.cpp \
    pageproductsoverview.cpp \
    pagethankyou.cpp \
    df_util.cpp \
    drinkorder.cpp \
    dbmanager.cpp \
    page_maintenance.cpp \
    wifiErrorPage.cpp \
    page_maintenance_dispenser.cpp \
    init.cpp \
    help.cpp \
    ../library/qr/qrcodegen.cpp

RESOURCES += \
    src.qrc

DISTFILES += \
    styles/product_select.qss  

LIBS += \
    -lcurl

# EV2 Reference for moneris
# TODO: Determine new linkage to object and library files...

#LIBS += -Lmoneris_obj/ -lmoneris
#OBJECTS += \
#     moneris_obj/moneriscommunication.o\
#     moneris_obj/packetfromecr.o\
#     moneris_obj/packetfromux410.o\
#     moneris_obj/transactionPackets.o\
#     moneris_obj/transactioninfo.o

#unix:!macx: LIBS += -L$$PWD/objm/ -lposm

#INCLUDEPATH += $$PWD/objm
#DEPENDPATH += $$PWD/objme

#unix:!macx: PRE_TARGETDEPS += $$PWD/objm/libposm.a

# Release Flag
#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT
