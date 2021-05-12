PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=3fdf1238987b6aeec113b1872e56307893feeae7
PKG_LICENSE=LGPL

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypt -f $(CURDIR)/Makefile.$(PKG_NAME)_crypt
