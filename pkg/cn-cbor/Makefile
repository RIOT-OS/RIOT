PKG_NAME=cn-cbor
PKG_URL=https://github.com/cabo/cn-cbor
PKG_VERSION=2f9c3b1931eb012909e74f3b628e6a31fd446ad1
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.cn-cbor

include $(RIOTBASE)/pkg/pkg.mk
