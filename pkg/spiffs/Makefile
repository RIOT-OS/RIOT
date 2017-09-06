PKG_NAME=spiffs
PKG_URL=https://github.com/pellepl/spiffs.git
PKG_VERSION=287148c46587089c4543a21eef2d6e9e14b88364
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
