PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=f08ff974c03e5c848bc5d2ae3fddb6a97897881a
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	cp $(RIOTBASE)/pkg/u8g2/src/Makefile $(PKG_BUILDDIR)/Makefile
	cp $(RIOTBASE)/pkg/u8g2/src/csrc/Makefile $(PKG_BUILDDIR)/csrc/Makefile
	cp $(RIOTBASE)/pkg/u8g2/src/csrc/u8g2_riotos.c $(PKG_BUILDDIR)/csrc/u8g2_riotos.c
	cp $(RIOTBASE)/pkg/u8g2/src/sys/sdl/common/Makefile $(PKG_BUILDDIR)/sys/sdl/common/Makefile
	cp $(RIOTBASE)/pkg/u8g2/src/sys/utf8/common/Makefile $(PKG_BUILDDIR)/sys/utf8/common/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
