PKG_NAME=edbg
PKG_URL=https://github.com/ataradov/edbg
PKG_VERSION=d499ffd8297233b65dd91fd58fb98b9f9a03fec1
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR=$(CURDIR)/bin

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)
	mv $(PKG_BUILDDIR)/edbg .

include $(RIOTBASE)/pkg/pkg.mk
