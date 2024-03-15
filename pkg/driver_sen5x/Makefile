PKG_NAME=driver_sen5x
PKG_URL=https://github.com/Sensirion/embedded-i2c-sen5x
PKG_VERSION=d3a54b1ffacacab1cacb1b373c1c9ea00de3b4eb
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all

all:
	$(QQ) "$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/$(PKG_NAME).mk