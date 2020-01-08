PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=da74b3c91570b9716fbb424e90935806b2b29814
PKG_LICENSE=EDL-1.0,EPL-1.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR)

$(PKG_PREPARED): $(PKG_BUILDDIR)/copied
$(PKG_BUILDDIR)/copied: $(PKG_PATCHED) FORCE
	mkdir -p "$(PKG_BUILDDIR)"
	# copy the Wakaama core files
	cp $(PKG_BUILDDIR)/core/*.c $(PKG_BUILDDIR)/core/*.h $(PKG_BUILDDIR)
	# copy the coap implementation from Wakaama
	cp $(PKG_BUILDDIR)/core/er-coap-13/*.c $(PKG_BUILDDIR)/core/er-coap-13/*.h $(PKG_BUILDDIR)
	# copy the mandatory objects, implemented on Wakaama examples
	cp $(PKG_BUILDDIR)/examples/client/object_server.c $(PKG_BUILDDIR)
	cp $(PKG_BUILDDIR)/examples/client/object_security.c $(PKG_BUILDDIR)
	cp $(PKG_BUILDDIR)/examples/client/object_access_control.c $(PKG_BUILDDIR)

	echo 'MODULE:=wakaama' > $(PKG_BUILDDIR)/Makefile
	echo 'include $$(RIOTBASE)/Makefile.base' >> $(PKG_BUILDDIR)/Makefile