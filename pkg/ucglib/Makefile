PKG_NAME=ucglib
PKG_URL=https://github.com/olikraus/ucglib
PKG_VERSION=bf48515702dd7c87cbacdd17989738f33f003df2
PKG_LICENSE=BSD-2-Clause

.PHONY: all

all: git-download
	cp $(RIOTBASE)/pkg/ucglib/src/Makefile $(PKG_BUILDDIR)/Makefile
	cp $(RIOTBASE)/pkg/ucglib/src/csrc/Makefile $(PKG_BUILDDIR)/csrc/Makefile
	cp $(RIOTBASE)/pkg/ucglib/src/csrc/ucg_riotos.c $(PKG_BUILDDIR)/csrc/ucg_riotos.c
	cp $(RIOTBASE)/pkg/ucglib/src/sys/sdl/dev/Makefile $(PKG_BUILDDIR)/sys/sdl/dev/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)

include $(RIOTBASE)/pkg/pkg.mk
