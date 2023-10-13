PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = 54e8938e60ba6a74d8aeafffc2d4a1dfc0392498
PKG_LICENSE = CC-0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
