QT += core gui sql widgets serialport network svg quick qml dbus

TARGET = DF_UI

CONFIG += \
    c++11

FORMS += \
    dispensepage.ui \
    idle.ui \
    paypage.ui \
    payselect.ui \
    productpage_1.ui \
    productpage_2.ui \
    thankyoupage.ui \
    maintenancePage.ui \
    maintain_product.ui \
    init.ui \
    help.ui \
    keyboard.ui

HEADERS += \
    dfuicommthread.h \
    dfuiserver.h \
    dispensepage.h \
    idle.h \
    includefiles.h \
    paypage.h \
    payselect.h \
    posm/enums.h \
    posm/lrcgenerator.h \
    posm/mcommunication.h \
    posm/packetfromecr.h \
    posm/packetfromux410.h \
    posm/packetreadsort.h \
    posm/paymentdatabase.h \
    posm/transactionPackets.h \
    posm/transactioninfo.h \
    productpage_1.h \
    productpage_2.h \
    thankyoupage.h \
    df_util.h \
    drinkorder.h \
    dbmanager.h \
    maintenancePage.h \
    maintain_product.h \
    init.h \
    help.h

SOURCES += \
    dfuicommthread.cpp \
    dfuiserver.cpp \
    dispensepage.cpp \
    idle.cpp \
    main.cpp \
    paypage.cpp \
    payselect.cpp \
    posm/lrcgenerator.cpp \
    posm/mcommunication.cpp \
    posm/packetfromecr.cpp \
    posm/packetfromux410.cpp \
    posm/packetreadsort.cpp \
    posm/paymentdatabase.cpp \
    posm/transactionPackets.cpp \
    posm/transactioninfo.cpp \
    productpage_1.cpp \
    productpage_2.cpp \
    thankyoupage.cpp \
    df_util.cpp \
    drinkorder.cpp \
    dbmanager.cpp \
    maintenance.cpp \
    maintain_product.cpp \
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
