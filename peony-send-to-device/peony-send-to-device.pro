QT += core gui widgets concurrent

include(../common.pri)

TARGET = peony-send-to-device
TEMPLATE = lib

DEFINES += PEONYADMIN_LIBRARY

PKGCONFIG += peony

CONFIG += link_pkgconfig no_keywords c++11 plugin debug


SOURCES += \
    send-to-device-plugin.cpp
HEADERS += \
    send-to-device-plugin.h

TRANSLATIONS += \
    translations/peony-send-to-device_cs.ts \
    translations/peony-send-to-device_tr.ts \
    translations/peony-send-to-device_zh_CN.ts

RESOURCES += \
    resources.qrc

target.path = $$[QT_INSTALL_LIBS]/peony-extensions

INSTALLS += target
