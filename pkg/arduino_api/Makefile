PKG_NAME=arduino_api
PKG_URL=https://github.com/arduino/ArduinoCore-API
PKG_VERSION=e03b65374c614130aa1b11597e07b3b5089a726d
PKG_LICENSE=LGPL-2.1

GITAMFLAGS = --3way

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/api -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
