PKG_NAME=flashdb
PKG_URL=https://github.com/armink/FlashDB.git
# 2.1.1
PKG_VERSION=714d6159e7e6afb267a3953756abca445c350e61
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-pedantic
CFLAGS += -Wno-cast-align
CFLAGS += -Wno-unused
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-format

.PHONY: flashdb_fal

all: $(filter flashdb_fal, $(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.flashdb

flashdb_fal:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/port/fal/src -f $(CURDIR)/Makefile.fal
