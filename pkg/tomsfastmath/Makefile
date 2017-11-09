PKG_NAME = tomsfastmath
PKG_URL = https://github.com/libtom/tomsfastmath.git
PKG_VERSION = e3b0c71363179c51d539005c2b0452e5b3ff04b6 # v0.13.1

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)/src
include $(RIOTBASE)/pkg/pkg.mk
