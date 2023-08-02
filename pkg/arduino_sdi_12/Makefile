PKG_NAME=arduino_sdi_12
PKG_URL=https://github.com/EnviroDIY/Arduino-SDI-12
PKG_VERSION=cd1349612f7b38f7a3cf629ab96d16dfd023be80
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
