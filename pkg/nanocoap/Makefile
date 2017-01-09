PKG_NAME=nanocoap
PKG_URL=https://github.com/kaspar030/sock
PKG_VERSION=cf2aff1b4322cee06bb5b24d0215bb764372fba7
PKG_LICENSE=LGPL-2.1

.PHONY: all

all: git-download
	@cp Makefile.nanocoap $(PKG_BUILDDIR)/nanocoap/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/nanocoap

include $(RIOTBASE)/pkg/pkg.mk
