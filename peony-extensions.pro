TEMPLATE = subdirs

VERSION = 3.2.2
DEFINES += VERSION='\\"$${VERSION}\\"'

SUBDIRS = \
#    peony-admin                         \
    peony-share                         \
    peony-drive-rename                  \
    peony-send-to-device                \
    peony-set-wallpaper                 \
    peony-bluetooth-plugin              \
    peony-engrampa-menu-plugin          \
    peony-extension-computer-view       \
    peony-menu-plugin-mate-terminal     \
#    test-computer-view
