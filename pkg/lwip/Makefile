PKG_NAME=lwip
PKG_URL=git://git.savannah.nongnu.org/lwip.git
# lwIP v2.1.0
PKG_VERSION=e6a8415df332ee34d7af02255b2aa1e8ee74348f
PKG_LICENSE=BSD-3-Clause

LWIP_MODULES         = lwip_api lwip_core lwip_ipv4 lwip_ipv6 \
                       lwip_netif lwip_netif_ppp lwip_polarssl
LWIP_USEMODULE       = $(filter $(LWIP_MODULES),$(USEMODULE))
LWIP_MODULE_MAKEFILE = $(RIOTBASE)/Makefile.base

.PHONY: all $(LWIP_MODULES)

CFLAGS += -Wno-address

make_module = "$(MAKE)" -f $(LWIP_MODULE_MAKEFILE) MODULE=$(1) -C $(2)

all: git-download lwip

lwip: $(LWIP_USEMODULE)
	$(call make_module,$@,$(PKG_BUILDDIR))

lwip_api:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/api)

lwip_core:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/core)

lwip_ipv4:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/core/ipv4)

lwip_ipv6:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/core/ipv6)

lwip_netif:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/netif)

lwip_netif_ppp:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/netif/ppp)

lwip_polarssl:
	$(call make_module,$@,$(PKG_BUILDDIR)/src/netif/ppp/polarssl)

include $(RIOTBASE)/pkg/pkg.mk
