TEMPLATE = subdirs

VERSION = 3.1.0
DEFINES += VERSION='\\"$${VERSION}\\"'

SUBDIRS = peony-admin \
          #peony-bluetooth-plugin \
	  peony-engrampa-menu-plugin \
	  peony-extension-computer-view \
	  peony-menu-plugin-mate-terminal \
	  peony-set-wallpaper \
	  peony-share
