PKG_NAME=wakaama
PKG_URL=git://github.com/eclipse/wakaama.git
PKG_VERSION=69a32cfae39f66fe4eec4cc8d1cd48ced7ad447c

.PHONY: all

all: patch
	"$(MAKE)" -C $(PKG_BUILDDIR)/riotbuild

patch: git-download
	mkdir -p "$(PKG_BUILDDIR)/riotbuild"
	cp $(PKG_BUILDDIR)/core/*.c $(PKG_BUILDDIR)/core/*.h $(PKG_BUILDDIR)/riotbuild
	cp $(PKG_BUILDDIR)/core/er-coap-13/*.c $(PKG_TEMP_DIR)/core/er-coap-13/*.h $(PKG_BUILDDIR)/riotbuild

	echo 'MODULE:=wakaama' > $(PKG_BUILDDIR)/riotbuild/Makefile
	echo 'include $$(RIOTBASE)/Makefile.base' >> $(PKG_BUILDDIR)/riotbuild/Makefile

include $(RIOTBASE)/pkg/pkg.mk
