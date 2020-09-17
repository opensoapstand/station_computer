QT += core gui sql widgets serialport network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    thankyoupage.ui

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
    drinkorder.h

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
    drinkorder.cpp

RESOURCES += \
    src.qrc

DISTFILES += \
    light/1_welcome_panel.jpg \
    light/2_drink_page_1.jpg \
    light/3_drink_page_2.jpg \
    light/4_pay_select_page_l.png \
    light/4_pay_select_page_l_1.jpg \
    light/4_pay_select_page_l_2.jpg \
    light/4_pay_select_page_l_3.jpg \
    light/4_pay_select_page_l_4.jpg \
    light/4_pay_select_page_l_5.jpg \
    light/4_pay_select_page_s.jpg \
    light/4_pay_select_page_s_1.jpg \
    light/4_pay_select_page_s_2.jpg \
    light/4_pay_select_page_s_3.jpg \
    light/4_pay_select_page_s_4.jpg \
    light/4_pay_select_page_s_5.jpg \
    light/5_pay_page_blank.jpg \
    light/5_pay_page_l_1.jpg \
    light/5_pay_page_l_2.jpg \
    light/5_pay_page_l_3.jpg \
    light/5_pay_page_l_4.jpg \
    light/5_pay_page_l_5.jpg \
    light/5_pay_page_s_1.jpg \
    light/5_pay_page_s_2.jpg \
    light/5_pay_page_s_3.jpg \
    light/5_pay_page_s_4.jpg \
    light/5_pay_page_s_5.jpg \
    light/6_dispense_page.png \
    light/7_thank_you_page.png \
    light/background.png \
    light/bottom_1.jpg \
    light/bottom_2.jpg \
    styles/product_select.qss

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
