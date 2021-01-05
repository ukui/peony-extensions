QT += core gui widgets concurrent

include(../common.pri)

TARGET = peony-admin
TEMPLATE = lib

DEFINES += PEONYADMIN_LIBRARY

PKGCONFIG += peony
CONFIG += link_pkgconfig \
          c++11 \
	  plugin \
	  debug

#DESTDIR += ../testdir


PEONY_PATH = "/usr/bin/peony"
PLUMA_PATH = "/usr/bin/pluma"
TERMINAL_PATH = "/usr/bin/mate-terminal"

policy.input = org.freedesktop.peony-admin.policy.in
policy.output = org.freedesktop.peony-admin.policy
policy.config = verbatim
QMAKE_SUBSTITUTES += policy

policy_file.files = org.freedesktop.peony-admin.policy
policy_file.path = /usr/share/polkit-1/actions

SOURCES += admin-menu-plugin.cpp
HEADERS += admin-menu-plugin.h

TRANSLATIONS += translations/peony-admin_cs.ts \
                translations/peony-admin_tr.ts \
		translations/peony-admin_zh_CN.ts

RESOURCES += resources.qrc

target.path = $$[QT_INSTALL_LIBS]/peony-extensions
INSTALLS += target policy_file
