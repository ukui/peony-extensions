QT += widgets

TARGET = peony-qt-computer-view-plugin

TEMPLATE = lib
DEFINES += PEONYQTEXTENSIONCOMPUTERVIEW_LIBRARY

CONFIG += debug c++11 no_keywords link_pkgconfig plugin
PKGCONFIG +=gio-2.0 glib-2.0 gio-unix-2.0 peony

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS = peony-qt-extension-computer-view-zh_CN.ts

include(computer-view/computer-view.pri)

SOURCES += \
    computer-view-container.cpp \
    login-remote-filesystem.cpp \
    peony-computer-view-plugin.cpp \

HEADERS += \
    computer-view-container.h \
    login-remote-filesystem.h \
    peony-computer-view-plugin.h \
    peony-qt-extension-computer-view_global.h \

DESTDIR += ../../testdir

# Default rules for deployment.
unix {
    target.path = /usr/lib/peony-qt-extensions
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    login-remote-filesystem.ui

RESOURCES += \
    translation.qrc
