PKG_NAME=arduino_api
PKG_URL=https://github.com/arduino/ArduinoCore-API
PKG_VERSION=173e8eadced2ad32eeb93bcbd5c49f8d6a055ea6
PKG_LICENSE=LGPL-2.1

GITAMFLAGS = --3way

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/api -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
