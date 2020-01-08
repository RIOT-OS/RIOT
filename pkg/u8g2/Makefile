PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=f08ff974c03e5c848bc5d2ae3fddb6a97897881a
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	cp -u src/Makefile $(PKG_BUILDDIR)/Makefile
	cp -u src/csrc/Makefile $(PKG_BUILDDIR)/csrc/Makefile
	cp -u src/csrc/u8g2_riotos.c $(PKG_BUILDDIR)/csrc/u8g2_riotos.c
	cp -u src/sys/sdl/common/Makefile $(PKG_BUILDDIR)/sys/sdl/common/Makefile
	cp -u src/sys/utf8/common/Makefile $(PKG_BUILDDIR)/sys/utf8/common/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)
