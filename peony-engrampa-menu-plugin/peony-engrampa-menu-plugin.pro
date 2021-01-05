#-------------------------------------------------
#
# Project created by QtCreator 2019-11-27T09:45:26
#
#-------------------------------------------------

QT       += widgets

TARGET = peony-engrampa-menu-plugin
TEMPLATE = lib

DEFINES += PEONYENGRAMPAMENUPLUGIN_LIBRARY

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

PKGCONFIG += peony

TRANSLATIONS = translations/peony-engrampa-menu_zh_CN.ts \
               translations/peony-engrampa-menu_tr.ts \
	       translations/peony-engrampa-menu_cs.ts

#DESTDIR += ../testdir

SOURCES += \
    engrampa-menu-plugin.cpp

HEADERS += \
        engrampa-menu-plugin.h \
        peony-engrampa-menu-plugin_global.h 

unix {
    target.path = $$[QT_INSTALL_LIBS]/peony-extensions
    INSTALLS += target
}

RESOURCES += \
    peony-engrampa-menu-plugin.qrc
