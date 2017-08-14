PKG_NAME=minmea
PKG_URL=https://github.com/kaspar030/minmea
PKG_VERSION=f3253039a32af98924b0606316a83c8129dff4d4
PKG_LICENSE=WTFPL

.PHONY: all

all: git-download
	@cp Makefile.${PKG_NAME} $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
