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
    page_payment_tap_tcp.ui \
    page_payment_tap_serial.ui \
    page_product.ui \
    page_product_mixing.ui \
    page_product_menu.ui \
    page_select_product.ui \
    page_end.ui \
    page_maintenance.ui \
    page_productOverview.ui \
    page_productFreeSample.ui \
    page_maintenance_dispenser.ui \
    page_init.ui \
    page_help.ui \
    page_email.ui \
    keyboard.ui \
    page_sendFeedback.ui \
    page_transactions.ui \
    page_maintenance_general.ui \
    page_idle_products.ui \
    statusbar.ui \
    page_buybottle.ui \
    input_widget.ui \
    page_how_to.ui

HEADERS += \
    dfuicommthread.h \
    dfuiserver.h \
    page_dispenser.h \
    page_idle.h \
    includefiles.h \
    page_qr_payment.h \
    page_payment_tap_tcp.h \
    page_payment_tap_serial.h \
    page_product.h \
    page_product_mixing.h \
    posm/enums.h \
    posm/lrcgenerator.h \
    posm/mcommunication.h \
    page_productOverview.h \
    page_productFreeSample.h \
    posm/packetfromecr.h \
    posm/packetfromux410.h \
    posm/packetreadsort.h \
    posm/transactionPackets.h \
    posm/transactioninfo.h \
    page_product_menu.h \
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
    page_email.h \
    page_sendFeedback.h  \
    page_transactions.h \
    dispenser_slot.h \
    page_idle_products.h \
    machine.h \
    statusbar.h \
    keyboard.h \
    pnumberproduct.h \
    page_buybottle.h \
    input_widget.h \
    page_how_to.h

SOURCES += \
    dfuicommthread.cpp \
    dfuiserver.cpp \
    page_dispenser.cpp \
    page_idle.cpp \
    main.cpp \
    page_qr_payment.cpp \
    page_payment_tap_tcp.cpp \
    page_payment_tap_serial.cpp \
    page_product.cpp \
    page_product_mixing.cpp \
    posm/lrcgenerator.cpp \
    posm/mcommunication.cpp \
    page_productOverview.cpp \
    page_productFreeSample.cpp \
    posm/packetfromecr.cpp \
    posm/packetfromux410.cpp \
    posm/packetreadsort.cpp \
    posm/transactionPackets.cpp \
    posm/transactioninfo.cpp \
    page_product_menu.cpp \
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
    page_email.cpp \
    ../library/qr/qrcodegen.cpp \
    page_transactions.cpp \
    dispenser_slot.cpp \
    page_idle_products.cpp \
    machine.cpp \
    statusbar.cpp \
    keyboard.cpp \
    pnumberproduct.cpp \
    page_buybottle.cpp \
    input_widget.cpp \
    page_how_to.cpp

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
