PKG_NAME=nanocoap
PKG_URL=https://github.com/kaspar030/sock
PKG_VERSION=fc0a9470536bc760998827a6dc05d7dedf7b93ad
PKG_LICENSE=LGPL-2.1

.PHONY: all

all: git-download
	@cp Makefile.nanocoap $(PKG_BUILDDIR)/nanocoap/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/nanocoap

include $(RIOTBASE)/pkg/pkg.mk
