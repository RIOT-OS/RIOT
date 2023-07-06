PKG_NAME=driver_sen5x
PKG_URL=https://github.com/Sensirion/embedded-i2c-sen5x
PKG_VERSION=448a80ec490858f9fdec559fc95c3df52f0400c0
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	$(QQ) "$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/$(PKG_NAME).mk