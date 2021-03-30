PKG_NAME=lwip
PKG_URL=https://github.com/lwip-tcpip/lwip.git
# lwIP v2.1.2
PKG_VERSION=159e31b689577dbf69cf0683bbaffbd71fa5ee10
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

LWIP_MODULES         = lwip_api lwip_core lwip_ipv4 lwip_ipv6 \
                       lwip_netif lwip_netif_ppp lwip_polarssl
LWIP_USEMODULE       = $(filter $(LWIP_MODULES),$(USEMODULE))
LWIP_MODULE_MAKEFILE = $(RIOTBASE)/Makefile.base

.PHONY: $(LWIP_MODULES)

CFLAGS += -Wno-address

make_module = +$(QQ)"$(MAKE)" -C $(2) -f $(LWIP_MODULE_MAKEFILE) MODULE=$(1)

all: lwip

lwip: $(LWIP_USEMODULE)
	$(call make_module,$@,$(PKG_SOURCE_DIR))

lwip_api:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/api)

lwip_core:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/core)

lwip_ipv4:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/core/ipv4)

lwip_ipv6:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/core/ipv6)

lwip_netif:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/netif)

lwip_netif_ppp:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/netif/ppp)

lwip_polarssl:
	$(call make_module,$@,$(PKG_SOURCE_DIR)/src/netif/ppp/polarssl)
