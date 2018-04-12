PKG_NAME=cn-cbor
PKG_URL=https://github.com/cabo/cn-cbor
PKG_VERSION=f1cf9ffdf5cfab935a45900556f9b68af925c256
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.cn-cbor

include $(RIOTBASE)/pkg/pkg.mk
