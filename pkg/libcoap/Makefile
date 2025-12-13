PKG_NAME=libcoap
PKG_URL=https://github.com/obgm/libcoap
# develop branch as of 22nd April 2026
PKG_VERSION=244b0c2be4b3d30e62f0c13a51a845046b3e69ac
PKG_LICENSE=BSD-2-Clause

LIBCOAP_BUILD_DIR=$(BINDIR)/pkg/$(PKG_NAME)
LIBCOAP_SOURCE_DIR=$(RIOTBASE)/build/pkg/$(PKG_NAME)
LIBCOAP_INCLUDE_DIR=$(RIOTBASE)/build/pkg/$(PKG_NAME)/include/coap3

include $(RIOTBASE)/pkg/pkg.mk

LIBCOAP_MODULES = libcoap

ifneq (,$(filter libcoap_oscore,$(USEMODULE)))
    LIBCOAP_MODULES += libcoap_oscore
endif

all: $(LIBCOAP_MODULES)

libcoap:
	$(QQ)@cp $(LIBCOAP_SOURCE_DIR)/coap_config.h.riot $(LIBCOAP_SOURCE_DIR)/coap_config.h
	$(QQ)"$(MAKE)" -C $(LIBCOAP_SOURCE_DIR)/src -f $(CURDIR)/Makefile.libcoap

libcoap_oscore:
	$(QQ)"$(MAKE)" -C $(LIBCOAP_SOURCE_DIR)/src/oscore -f $(CURDIR)/Makefile.oscore
