TEMPLATE = app
QT += core gui sql virtualkeyboard

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
        main.cpp \
        survey.cpp

FORMS += \
    survey.ui

HEADERS += \
    survey.h

RESOURCES += \
    assets.qrc

DISTFILES += \
    ../../../../Qt/5.13.0/gcc_64/qml/QtQuick/VirtualKeyboard/Styles/test/style.qml
