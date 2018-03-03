PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=4dc79943020d9512207aea4cf914740556173793
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
