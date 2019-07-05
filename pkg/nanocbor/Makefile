PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = 44d9b1cfaa6829c5d1063f3c0e4a1733cfa409e3
PKG_LICENSE = LGPL-2.1

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src \
		-f $(RIOTPKG)/nanocbor/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
