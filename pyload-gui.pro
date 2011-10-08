#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T00:55:07
#
#-------------------------------------------------

QT       += core gui

TARGET = pyload-gui
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/thriftinterface/pyload_types.cpp \
    src/thriftinterface/pyload_constants.cpp \
    src/thriftinterface/Pyload.cpp \
    src/thriftclient.cpp \
    src/connectionmanager.cpp

HEADERS  += src/mainwindow.h \
    src/thriftinterface/pyload_types.h \
    src/thriftinterface/pyload_constants.h \
    src/thriftinterface/Pyload.h \
    src/thriftclient.h \
    src/connectionmanager.h \
    src/thrift/transport/TSocket.h

FORMS    += src/mainwindow.ui \
    src/connectionmanager.ui

RESOURCES += src/resources.qrc

LIBS += /usr/local/lib/libthrift.a

INCLUDEPATH += /usr/local/include/thrift

OTHER_FILES += \
    src/pyload.thrift \
    src/generateThrift.sh

