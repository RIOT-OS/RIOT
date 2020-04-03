PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = 3a672f79b2458a96393447e50a41174f741eadc5
PKG_LICENSE = LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/src \
		-f $(RIOTPKG)/nanocbor/Makefile.$(PKG_NAME)
