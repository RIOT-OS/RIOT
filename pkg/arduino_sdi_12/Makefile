PKG_NAME=arduino_sdi_12
PKG_URL=https://github.com/EnviroDIY/Arduino-SDI-12
PKG_VERSION=1fa8edbeb25fbb6cff71495583e664b5e8ae18a5
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
