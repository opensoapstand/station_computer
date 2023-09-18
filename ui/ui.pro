QT += core gui sql widgets serialport network svg quick qml dbus

TARGET = DF_UI

CONFIG += \
    c++11

QT += multimedia \ 
    multimediawidgets \
    widgets 

FORMS += \
    page_dispenser.ui \
    page_error_wifi.ui \
    page_idle.ui \
    page_qr_payment.ui \
    page_tap_payment.ui \
    page_tap_payment_serial.ui \
    page_product.ui \
    page_select_product.ui \
    page_end.ui \
    page_maintenance.ui \
    page_productOverview.ui \
    page_maintenance_dispenser.ui \
    page_init.ui \
    page_help.ui \
    keyboard.ui \
    page_sendFeedback.ui \
    page_transactions.ui \
    page_maintenance_general.ui \
    page_idle_products.ui

HEADERS += \
    dfuicommthread.h \
    dfuiserver.h \
    page_dispenser.h \
    page_idle.h \
    includefiles.h \
    page_qr_payment.h \
    page_tap_payment.h \
    page_tap_payment_serial.h \
    page_product.h \
    posm/enums.h \
    posm/lrcgenerator.h \
    posm/mcommunication.h \
    page_productOverview.h \
    posm/packetfromecr.h \
    posm/packetfromux410.h \
    posm/packetreadsort.h \
    posm/paymentdatabase.h \
    posm/transactionPackets.h \
    posm/transactioninfo.h \
    page_select_product.h \
    page_end.h \
    payment/commands.h \
    payment/setup_Tap.h \
    df_util.h \
    dbmanager.h \
    page_maintenance.h \
    page_maintenance_dispenser.h \
    page_maintenance_general.h \
    page_error_wifi.h \
    page_init.h \
    page_help.h \
    page_sendFeedback.h  \
    page_transactions.h \
    product.h \
    page_idle_products.h \
    machine.h

SOURCES += \
    dfuicommthread.cpp \
    dfuiserver.cpp \
    page_dispenser.cpp \
    page_idle.cpp \
    main.cpp \
    page_qr_payment.cpp \
    page_tap_payment.cpp \
    page_tap_payment_serial.cpp \
    page_product.cpp \
    posm/lrcgenerator.cpp \
    posm/mcommunication.cpp \
    page_productOverview.cpp \
    posm/packetfromecr.cpp \
    posm/packetfromux410.cpp \
    posm/packetreadsort.cpp \
    posm/paymentdatabase.cpp \
    posm/transactionPackets.cpp \
    posm/transactioninfo.cpp \
    page_select_product.cpp \
    page_sendFeedback.cpp \
    page_end.cpp \
    payment/commands.cpp \
    payment/setup_Tap.cpp \
    df_util.cpp \
    dbmanager.cpp \
    page_maintenance.cpp \
    page_error_wifi.cpp \
    page_maintenance_dispenser.cpp \
    page_maintenance_general.cpp \
    page_init.cpp \
    page_help.cpp \
    ../library/qr/qrcodegen.cpp \
    page_transactions.cpp \
    product.cpp \
    page_idle_products.cpp \
    machine.cpp

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
unix:!macx: LIBS += -L$$PWD/objm/ -lssl -lcrypto

#unix:!macx: LIBS += -L$$PWD/objm/ -lposm

#INCLUDEPATH += $$PWD/objm
#DEPENDPATH += $$PWD/objme

#unix:!macx: PRE_TARGETDEPS += $$PWD/objm/libposm.a

# Release Flag
#CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT