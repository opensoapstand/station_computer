TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        local_db_logger.cpp \
        main.cpp

HEADERS += \
    local_db_logger.h \

QMAKE_CXXFLAGS += `mysql_config --cflags --libs`
LIBS += $$system(mysql_config --libs)

