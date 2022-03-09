PKG_NAME=arduino_sdi_12
PKG_URL=https://github.com/EnviroDIY/Arduino-SDI-12
PKG_VERSION=fd9699b390edeac3a8681e2a6d4fe2ba8b1f9a51
PKG_LICENSE=BSD-3-Clause

GITAMFLAGS = --3way

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
