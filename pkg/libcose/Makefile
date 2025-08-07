PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=ea1fed87d6ca9b478f8bed323af97e6b192c0a6d
PKG_LICENSE=LGPL-2.1-only

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypt -f $(CURDIR)/Makefile.$(PKG_NAME)_crypt
