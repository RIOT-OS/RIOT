PKG_NAME=spiffs
PKG_URL=https://github.com/pellepl/spiffs.git
PKG_VERSION=39937743fbbec4b82308ee08332bf9180408d23b
PKG_BUILDDIR ?= $(PKGDIRBASE)/$(PKG_NAME)

CFLAGS += -std=c11

.PHONY: all

all: git-download
	@mkdir -p "$(PKG_BUILDDIR)/riotbuild"
	@cp $(PKG_BUILDDIR)/src/*.c $(PKG_BUILDDIR)/src/*.h $(PKG_BUILDDIR)/riotbuild

	@echo 'MODULE:=spiffs' > $(PKG_BUILDDIR)/riotbuild/Makefile
	@echo 'include $$(RIOTBASE)/Makefile.base' >> $(PKG_BUILDDIR)/riotbuild/Makefile

	"$(MAKE)" -C $(PKG_BUILDDIR)/riotbuild

include $(RIOTBASE)/pkg/pkg.mk
