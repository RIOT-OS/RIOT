PKG_NAME=libcose
PKG_URL=https://github.com/bergzand/libcose
PKG_VERSION=8b5f651c3203682a2d98121cd3e5c844cb2b4c36
PKG_LICENSE=LGPL

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.libcose
	"$(MAKE)" -C $(PKG_BUILDDIR)/src/crypt -f $(CURDIR)/Makefile.libcose_crypt
