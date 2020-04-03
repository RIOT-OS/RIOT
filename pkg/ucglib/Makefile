PKG_NAME=ucglib
PKG_URL=https://github.com/olikraus/ucglib
PKG_VERSION=bf48515702dd7c87cbacdd17989738f33f003df2
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

all:
	cp -u src/Makefile $(PKG_BUILDDIR)/Makefile
	cp -u src/csrc/Makefile $(PKG_BUILDDIR)/csrc/Makefile
	cp -u src/csrc/ucg_riotos.c $(PKG_BUILDDIR)/csrc/ucg_riotos.c
	cp -u src/sys/sdl/dev/Makefile $(PKG_BUILDDIR)/sys/sdl/dev/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)
