PKG_NAME     = setsid
PKG_URL      = https://github.com/tzvetkoff/setsid-macosx
PKG_VERSION  = e5b851df41591021baf5cf88d4e41572baf8e08b
PKG_LICENSE  = BSD-2-Clause

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..

PKG_SOURCE_DIR = $(CURDIR)/bin
PKG_BUILD_OUT_OF_SOURCE = 0
include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/setsid

$(CURDIR)/setsid:
	@echo "[INFO] compiling setsid from source now"
	$(MAKE) BINDIR=$(PKG_BUILD_DIR) -C $(PKG_SOURCE_DIR)
	@mv $(PKG_SOURCE_DIR)/setsid $(CURDIR)/setsid

clean::
	rm -f $(CURDIR)/setsid
