PKG_NAME=wakaama
PKG_URL=https://github.com/eclipse/wakaama.git
PKG_VERSION=da74b3c91570b9716fbb424e90935806b2b29814
PKG_LICENSE=EDL-1.0,EPL-1.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/core -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/core/er-coap-13 -f $(RIOTBASE)/Makefile.base MODULE=wakaama_core_coap13
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/examples/client -f $(CURDIR)/wakaama_client.mk
