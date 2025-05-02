PKG_NAME=libcoap
PKG_URL=https://github.com/obgm/libcoap
PKG_VERSION=fada39dcba95297711014ad817e7c568066081d1
PKG_LICENSE=BSD-2-Clause

LIBCOAP_BUILD_DIR=$(BINDIR)/pkg/$(PKG_NAME)
LIBCOAP_SOURCE_DIR=$(RIOTBASE)/build/pkg/$(PKG_NAME)
LIBCOAP_INCLUDE_DIR=$(RIOTBASE)/build/pkg/$(PKG_NAME)/include/coap3

include $(RIOTBASE)/pkg/pkg.mk

ifneq (,$(filter libcoap_oscore,$(USEMODULE)))
    all: libcoap libcoap_oscore
else
    all: libcoap
endif

libcoap:
	$(QQ)@cp $(LIBCOAP_SOURCE_DIR)/coap_config.h.riot $(LIBCOAP_SOURCE_DIR)/coap_config.h
	$(QQ)"$(MAKE)" -C $(LIBCOAP_SOURCE_DIR)/src -f $(CURDIR)/Makefile.libcoap

libcoap_oscore:
	$(QQ)"$(MAKE)" -C $(LIBCOAP_SOURCE_DIR)/src/oscore -f $(CURDIR)/Makefile.oscore
