#-------------------------------------------------
#
# Project created by QtCreator 2020-11-04T19:34:17
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Library
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    tools/useraccount.cpp \
    database/dbhelper.cpp \
    pages/login.cpp \
    pages/welcome.cpp \
    pages/viewinfo.cpp \
    pages/lost.cpp \
    pages/found.cpp \
    pages/viewrecord.cpp \
    tools/serialportthread.cpp \
    tools/tools.cpp \
    pages/bind.cpp \
    pages/registor.cpp \
    pages/unregistor.cpp \
    pages/borrow.cpp

HEADERS += \
        mainwindow.h \
    pages/pages.h \
    tools/useraccount.h \
    database/dbhelper.h \
    inc/m1356dll.h \
    inc/m1356dll_global.h \
    pages/login.h \
    pages/welcome.h \
    pages/viewinfo.h \
    pages/lost.h \
    pages/found.h \
    pages/viewrecord.h \
    tools/serialportthread.h \
    tools/tools.h \
    pages/bind.h \
    pages/registor.h \
    pages/unregistor.h \
    pages/borrow.h

FORMS += \
        mainwindow.ui \
    pages/login.ui \
    pages/welcome.ui \
    pages/viewinfo.ui \
    pages/lost.ui \
    pages/found.ui \
    pages/viewrecord.ui \
    pages/bind.ui \
    pages/registor.ui \
    pages/unregistor.ui \
    pages/borrow.ui

RESOURCES += pics.qrc

win32: LIBS += -L$$PWD/lib/ -lM1356Dll

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/lib/M1356Dll.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/lib/libM1356Dll.a
