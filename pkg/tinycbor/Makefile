PKG_NAME=tinycbor
PKG_URL=https://github.com/intel/tinycbor
PKG_VERSION=v0.5.1
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.tinycbor

include $(RIOTBASE)/pkg/pkg.mk
