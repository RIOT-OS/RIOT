PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=ee80c224622684ee47c17c57918904cffd00c4d2
PKG_LICENSE=EDL-1.0,EPL-1.0

.PHONY: all

all: patch
	"$(MAKE)" -C $(PKG_BUILDDIR)/riotbuild

patch: git-download
	mkdir -p "$(PKG_BUILDDIR)/riotbuild"
	cp $(PKG_BUILDDIR)/core/*.c $(PKG_BUILDDIR)/core/*.h $(PKG_BUILDDIR)/riotbuild
	cp $(PKG_BUILDDIR)/core/er-coap-13/*.c $(PKG_BUILDDIR)/core/er-coap-13/*.h $(PKG_BUILDDIR)/riotbuild

	echo 'MODULE:=wakaama' > $(PKG_BUILDDIR)/riotbuild/Makefile
	echo 'include $$(RIOTBASE)/Makefile.base' >> $(PKG_BUILDDIR)/riotbuild/Makefile

include $(RIOTBASE)/pkg/pkg.mk
