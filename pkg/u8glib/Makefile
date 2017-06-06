PKG_NAME=u8glib
PKG_URL=https://github.com/olikraus/u8glib
PKG_VERSION=ec0acc2b7a0acc30194e736a4782aa2ad6802452

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
