PKG_NAME=flashdb
PKG_URL=https://github.com/armink/FlashDB.git
# 2.2.0
PKG_VERSION=65a87640bf816effc84ca161f5254878677232b8
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-pedantic
CFLAGS += -Wno-cast-align
CFLAGS += -Wno-unused
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-format

# File paths are built with snprintf() and checked for truncation afterwards,
# which newer GCC versions still report
CFLAGS += -Wno-format-truncation

.PHONY: flashdb_fal

all: $(filter flashdb_fal, $(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.flashdb

flashdb_fal:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/port/fal/src -f $(CURDIR)/Makefile.fal
