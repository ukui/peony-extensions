QT += core gui widgets concurrent

TARGET = peony-share
TEMPLATE = lib

DEFINES += PEONYSHARE_LIBRARY

include(../common.pri)

exists(/usr/include/peony-qt/usershare-manager.h) {
    message("usershare-manager.h for find.")
    DEFINES += LIBPEONY_USERSHARE_MANAGER=true
}

PKGCONFIG += peony
CONFIG += link_pkgconfig \
          c++11 \
	  plugin \
	  debug

#DESTDIR += ../testdir

SOURCES += share-page.cpp \
	   net-usershare-helper.cpp \
	   share-properties-page-plugin.cpp \
	   SwitchButton/switchbutton.cpp

HEADERS += share-page.h \
	   net-usershare-helper.h \
	   share-properties-page-plugin.h \
	   SwitchButton/switchbutton.h

TRANSLATIONS += translations/peony-share-extension_cs.ts \
                translations/peony-share-extension_tr.ts \
                translations/peony-share-extension_zh_CN.ts

RESOURCES += resources.qrc

target.path = $$[QT_INSTALL_LIBS]/peony-extensions

INSTALLS += target
