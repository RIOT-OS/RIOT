PKG_NAME=lz4
PKG_URL=https://github.com/lz4/lz4
# v1.9.4
PKG_VERSION=5ff839680134437dbf4678f3d0c7b371d84f4964
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -f $(CURDIR)/$(PKG_NAME).mk -C $(PKG_SOURCE_DIR)/lib
