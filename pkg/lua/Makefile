PKG_NAME=lua
PKG_URL=https://github.com/lua/lua.git
PKG_VERSION=e354c6355e7f48e087678ec49e340ca0696725b1
PKG_LICENSE=MIT

.PHONY: all

all:
	@cp Makefile.lua $(PKG_BUILDDIR)
	"$(MAKE)" -C $(PKG_BUILDDIR) -f Makefile.lua

include $(RIOTBASE)/pkg/pkg.mk
