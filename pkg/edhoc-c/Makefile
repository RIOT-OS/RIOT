PKG_NAME = EDHOC-C
PKG_URL = https://github.com/future-proof-iot/EDHOC-C.git
PKG_VERSION = 1847c2c558d3ea97a070f6a9546a5913432ecf96
PKG_LICENSE = BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: edhoc-c_%

EDHOC_C_MODULES := $(filter edhoc-c_%,$(USEMODULE))

all: $(EDHOC_C_MODULES)
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=edhoc-c

edhoc-c_crypto_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/crypto -f $(RIOTBASE)/Makefile.base MODULE=$@ SRC=$*.c

edhoc-c_cbor_%:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/cbor -f $(RIOTBASE)/Makefile.base MODULE=$@ SRC=$*.c
