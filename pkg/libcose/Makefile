PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=2929fdce7affbd5bb9db201370d95d8f7cf680f9
PKG_LICENSE=LGPL

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypt -f $(CURDIR)/Makefile.$(PKG_NAME)_crypt
