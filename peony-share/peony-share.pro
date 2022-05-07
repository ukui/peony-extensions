QT += core gui widgets concurrent

TARGET = peony-share
TEMPLATE = lib

DEFINES += PEONYSHARE_LIBRARY

include(../common.pri)

PKGCONFIG += peony
CONFIG += link_pkgconfig \
          c++11 \
	  plugin \
	  debug

#DESTDIR += ../testdir

SOURCES += share-page.cpp \
	   share-emblem-provider.cpp \
	   share-properties-page-plugin.cpp

HEADERS += share-page.h \
	   share-emblem-provider.h \
	   share-properties-page-plugin.h

TRANSLATIONS += translations/peony-share-extension_cs.ts \
                translations/peony-share-extension_tr.ts \
                translations/peony-share-extension_zh_CN.ts

RESOURCES += resources.qrc

target.path = $$[QT_INSTALL_LIBS]/peony-extensions

polkit1.files = $$PWD/org.freedesktop.peony-share.policy
polkit1.path = /usr/share/polkit-1/actions/

polkitbin.files = $$PWD/peony-share.sh
polkitbin.path = /usr/bin/

INSTALLS += target polkit1 polkitbin
