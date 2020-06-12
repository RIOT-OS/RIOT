PKG_NAME=ucglib
PKG_URL=https://github.com/olikraus/ucglib
PKG_VERSION=bf48515702dd7c87cbacdd17989738f33f003df2
PKG_LICENSE=BSD-2-Clause

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: ucglib_sdl

all: $(filter ucglib_sdl,$(USEMODULE))
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.ucglib
	"$(MAKE)" -C $(PKG_BUILDDIR)/csrc -f $(CURDIR)/Makefile.ucglib_csrc

ucglib_sdl:
	"$(MAKE)" -C $(PKG_BUILDDIR)/sys/sdl/dev -f $(CURDIR)/Makefile.ucglib_sdl
