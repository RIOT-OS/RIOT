PKG_NAME=u8g2
PKG_URL=https://github.com/olikraus/u8g2
PKG_VERSION=2e47ff05e7e7b38057fa875e14c4ecce2fb6a615
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	cp src/Makefile $(PKG_BUILDDIR)/Makefile
	cp src/csrc/Makefile $(PKG_BUILDDIR)/csrc/Makefile
	cp src/csrc/u8x8_riotos.c $(PKG_BUILDDIR)/csrc/u8x8_riotos.c
	cp src/csrc/u8x8_riotos.h $(PKG_BUILDDIR)/csrc/u8x8_riotos.h
	cp src/sys/sdl/common/Makefile $(PKG_BUILDDIR)/sys/sdl/common/Makefile
	cp src/sys/utf8/common/Makefile $(PKG_BUILDDIR)/sys/utf8/common/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)
