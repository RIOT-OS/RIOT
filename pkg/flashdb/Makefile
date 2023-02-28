PKG_NAME=flashdb
PKG_URL=https://github.com/armink/FlashDB.git
# 1.1.2
PKG_VERSION=7062902a3e7b6b8a7e8f5886ae242271a0164e05
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-pedantic
CFLAGS += -Wno-cast-align
CFLAGS += -Wno-unused
CFLAGS += -Wno-format

.PHONY: flashdb_fal

all: $(filter flashdb_fal, $(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.flashdb

flashdb_fal:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/port/fal/src -f $(CURDIR)/Makefile.fal
