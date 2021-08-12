QT += core gui widgets concurrent

include(../common.pri)

TARGET = peony-drive-rename
TEMPLATE = lib

DEFINES += PEONYADMIN_LIBRARY

PKGCONFIG += peony gio-2.0 udisks2 glib-2.0

CONFIG += link_pkgconfig no_keywords c++11 plugin debug


SOURCES += \
    drive-rename.cpp


HEADERS += \
    drive-rename.h


TRANSLATIONS += \

RESOURCES += \
    resource.qrc

target.path = $$[QT_INSTALL_LIBS]/peony-extensions

INSTALLS += target
