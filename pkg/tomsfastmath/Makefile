PKG_NAME = tomsfastmath
PKG_URL = https://github.com/libtom/tomsfastmath.git
PKG_VERSION = v0.13.0

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)/src
include $(RIOTBASE)/pkg/pkg.mk
