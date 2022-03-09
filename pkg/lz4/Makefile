PKG_NAME=lz4
PKG_URL=https://github.com/lz4/lz4
PKG_VERSION=d44371841a2f1728a3f36839fd4b7e872d0927d3  # v1.9.3
PKG_LICENSE=BSD

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -f $(CURDIR)/$(PKG_NAME).mk -C $(PKG_SOURCE_DIR)/lib
