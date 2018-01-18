PKG_NAME=mango
PKG_URL=https://github.com/ektrah/mango.git
PKG_VERSION=354bc92237841134a04b47dcbb839f2c09b6ee70
PKG_LICENSE=MIT License

.PHONY: all

# PKG_BUILDDIR ?= $(PKGDIRBASE)

all: git-download
	@cp Makefile.riot $(PKG_BUILDDIR)/src/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/src

include $(RIOTBASE)/pkg/pkg.mk
