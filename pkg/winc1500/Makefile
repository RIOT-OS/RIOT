PKG_NAME=winc1500
PKG_URL=https://github.com/kbumsik/winc1500-projects
PKG_VERSION=396ea0cb3d10ef7099a15f352a3a6d792b16b48b
PKG_LICENSE=BSD-3-Clause-Atmel

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/winc1500

include $(RIOTBASE)/pkg/pkg.mk
