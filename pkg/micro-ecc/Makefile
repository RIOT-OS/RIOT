PKG_NAME=micro-ecc
PKG_URL=https://github.com/kmackay/micro-ecc.git
PKG_VERSION=b6c0cdbe7d20af48b0c2a909a66ff00b093d1542
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
