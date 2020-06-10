PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=da74b3c91570b9716fbb424e90935806b2b29814
PKG_LICENSE=EDL-1.0,EPL-1.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" MODULE=wakaama_core -C $(PKG_BUILDDIR)/core -f $(RIOTBASE)/Makefile.base
	"$(MAKE)" MODULE=wakaama_core_coap13 -C $(PKG_BUILDDIR)/core/er-coap-13 -f $(RIOTBASE)/Makefile.base
	"$(MAKE)" -C $(PKG_BUILDDIR)/examples/client -f $(CURDIR)/wakaama_client.mk
