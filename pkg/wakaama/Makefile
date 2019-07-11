PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=da74b3c91570b9716fbb424e90935806b2b29814
PKG_LICENSE=EDL-1.0,EPL-1.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)/riotbuild -f $(CURDIR)/Makefile.wakaama

$(PKG_PREPARED): $(PKG_BUILDDIR)/riotbuild/copied
$(PKG_BUILDDIR)/riotbuild/copied: $(PKG_PATCHED) FORCE
	mkdir -p "$(PKG_BUILDDIR)/riotbuild"
	cp $(PKG_BUILDDIR)/core/*.c $(PKG_BUILDDIR)/core/*.h $(PKG_BUILDDIR)/riotbuild
	cp $(PKG_BUILDDIR)/core/er-coap-13/*.c $(PKG_BUILDDIR)/core/er-coap-13/*.h $(PKG_BUILDDIR)/riotbuild
	cp $(PKG_BUILDDIR)/examples/client/object_server.c $(PKG_BUILDDIR)/riotbuild
	cp $(PKG_BUILDDIR)/examples/client/object_security.c $(PKG_BUILDDIR)/riotbuild
	cp $(PKG_BUILDDIR)/examples/client/object_access_control.c $(PKG_BUILDDIR)/riotbuild
	touch $@
