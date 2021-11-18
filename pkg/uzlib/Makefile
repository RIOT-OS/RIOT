PKG_NAME=uzlib
PKG_URL=https://github.com/pfalcon/uzlib
PKG_VERSION=27e4f4c15ba30c2cfc89575159e8efb50f95037e  # v2.9.5
PKG_LICENSE=zlib

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-sign-compare

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/$(PKG_NAME).mk
