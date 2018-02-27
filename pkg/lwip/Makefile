PKG_NAME=lwip
PKG_URL=git://git.savannah.nongnu.org/lwip.git
PKG_VERSION=STABLE-2_0_3_RELEASE
PKG_LICENSE=BSD-3-Clause

LWIP_SUBMODULES := $(patsubst makefiles/%.mk,%,$(wildcard makefiles/lwip_*.mk))
LWIP_USEMODULE := $(filter $(LWIP_SUBMODULES),$(USEMODULE))

.PHONY: all $(LWIP_SUBMODULES)

all: git-download $(LWIP_USEMODULE)
	"$(MAKE)" -f $(CURDIR)/makefiles/Makefile.lwip -C $(PKG_BUILDDIR)

lwip_api: $(CURDIR)/makefiles/lwip_api.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/api

lwip_core: $(CURDIR)/makefiles/lwip_core.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/core

lwip_ipv4: $(CURDIR)/makefiles/lwip_ipv4.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/core/ipv4

lwip_ipv6: $(CURDIR)/makefiles/lwip_ipv6.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/core/ipv6

lwip_netif: $(CURDIR)/makefiles/lwip_netif.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/netif

lwip_netif_ppp: $(CURDIR)/makefiles/lwip_netif_ppp.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/netif/ppp

lwip_polarssl: $(CURDIR)/makefiles/lwip_polarssl.mk
	"$(MAKE)" -f $< -C $(PKG_BUILDDIR)/src/netif/ppp/polarssl

include $(RIOTBASE)/pkg/pkg.mk
