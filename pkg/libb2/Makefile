PKG_NAME    = libb2
PKG_URL     = https://github.com/BLAKE2/libb2
PKG_VERSION = 60ea749837362c226e8501718f505ab138e5c19d # v0.98
PKG_LICENSE = CC0-1.0

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)
