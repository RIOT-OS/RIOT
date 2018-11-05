PKG_NAME=tinycbor
PKG_URL=https://github.com/intel/tinycbor
# Tinycbor v0.5.2
PKG_VERSION=d94ca09aa91f5b3c581527aa8bca179a82b79874
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.tinycbor

include $(RIOTBASE)/pkg/pkg.mk
