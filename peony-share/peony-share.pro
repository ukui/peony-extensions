QT += core gui widgets concurrent

TARGET = peony-share
TEMPLATE = lib

DEFINES += PEONYSHARE_LIBRARY

PKGCONFIG += peony
CONFIG += link_pkgconfig \
          c++11 \
	  plugin \
	  debug

DESTDIR += ../../testdir

SOURCES += net-usershare-helper.cpp \
           share-page.cpp \
	   share-properties-page-plugin.cpp

HEADERS += net-usershare-helper.h \
           share-page.h \
	   share-properties-page-plugin.h

TRANSLATIONS += translations/peony-share-extension_cs.ts \
                translations/peony-share-extension_tr.ts \
                translations/peony-share-extension_zh_CN.ts

RESOURCES += resources.qrc

target.path = $$[QT_INSTALL_LIBS]/peony-extensions
INSTALLS += target
