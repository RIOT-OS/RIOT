PKG_NAME=tinycbor
PKG_URL=https://github.com/intel/tinycbor
# Tinycbor v0.6.0
PKG_VERSION=d393c16f3eb30d0c47e6f9d92db62272f0ec4dc7
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.tinycbor
