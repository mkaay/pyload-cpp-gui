#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T00:55:07
#
#-------------------------------------------------

QT       += core gui

TARGET = pyload-gui
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    thriftinterface/pyload_types.cpp \
    thriftinterface/pyload_constants.cpp \
    thriftinterface/Pyload.cpp \
    thriftclient.cpp \
    connectionmanager.cpp

HEADERS  += mainwindow.h \
    thriftinterface/pyload_types.h \
    thriftinterface/pyload_constants.h \
    thriftinterface/Pyload.h \
    thriftclient.h \
    connectionmanager.h

FORMS    += mainwindow.ui \
    connectionmanager.ui

RESOURCES += \
    resources.qrc

LIBS += /usr/local/lib/libthrift.a\

INCLUDEPATH += /usr/local/include/thrift

OTHER_FILES += \
    pyload.thrift \
    generateThrift.sh
