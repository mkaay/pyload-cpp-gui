#-------------------------------------------------
#
# Project created by QtCreator 2011-06-10T00:55:07
#
#-------------------------------------------------

QT       += core gui

TARGET = pyload-gui
TEMPLATE = app


SOURCES += \
    src/ui/mainwindow/mainwindow.cpp \
    src/file.cpp \
    src/package.cpp \
    src/eventloop.cpp \
    src/thrift/interface/pyload_types.cpp \
    src/thrift/interface/pyload_constants.cpp \
    src/thrift/interface/Pyload.cpp \
    src/thrift/client.cpp \
    src/ui/downloadsmodel.cpp \
    src/ui/progressdelegate.cpp \
    src/ui/connectionmanager/connectionmanager.cpp \
    src/application.cpp \
    src/main.cpp

HEADERS  += \
    src/ui/mainwindow/mainwindow.h \
    src/file.h \
    src/package.h \
    src/eventloop.h \
    src/thrift/interface/pyload_types.h \
    src/thrift/interface/pyload_constants.h \
    src/thrift/interface/Pyload.h \
    src/thrift/client.h \
    src/thrift/transport/TSocket.h \
    src/ui/downloadsmodel.h \
    src/ui/progressdelegate.h \
    src/ui/connectionmanager/connectionmanager.h \
    src/application.h

FORMS    += src/ui/mainwindow/mainwindow.ui \
    src/ui/connectionmanager/connectionmanager.ui

RESOURCES += resources/resources.qrc

LIBS += /usr/local/lib/libthrift.a

INCLUDEPATH += /usr/local/include/thrift

OTHER_FILES += \
    src/thrift/pyload.thrift \
    tools/generateThrift.sh \
    tools/refreshInterface.sh























