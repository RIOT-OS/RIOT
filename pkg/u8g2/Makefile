PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=4c7ecf099e766b9c678d3453d6b932c8290bdb6b

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
