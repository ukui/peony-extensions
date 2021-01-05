#-------------------------------------------------
#
# Project created by QtCreator 2020-06-11T13:45:26
#
#-------------------------------------------------

QT       += widgets

TARGET = peony-set-wallpaper
TEMPLATE = lib

DEFINES += PEONYSETWALLPAPER_LIBRARY

include(../common.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += debug link_pkgconfig plugin

PKGCONFIG += peony gsettings-qt

TRANSLATIONS = translations/peony-set-wallpaper-extension_zh_CN.ts \
               translations/peony-set-wallpaper-extension_tr.ts

#DESTDIR += ../testdir

SOURCES += \
    set-wallpaper-plugin.cpp

HEADERS += \
        set-wallpaper-plugin.h \
        peony-set-wallpaper_global.h

unix {
    target.path = $$[QT_INSTALL_LIBS]/peony-extensions
    INSTALLS += target
}

RESOURCES += \
    peony-set-wallpaper.qrc
