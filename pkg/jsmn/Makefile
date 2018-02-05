PKG_NAME=jsmn
PKG_URL=https://github.com/zserge/jsmn
PKG_VERSION=6784c826d9674915a4d89649c6288e6aecb4110d
PKG_LICENSE=MIT

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.jsmn

include $(RIOTBASE)/pkg/pkg.mk
