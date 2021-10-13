PKG_NAME    = libb2
PKG_URL     = https://github.com/BLAKE2/libb2
PKG_VERSION = 2c5142f12a2cd52f3ee0a43e50a3a76f75badf85 # v0.98.1
PKG_LICENSE = CC0-1.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)
