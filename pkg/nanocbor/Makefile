PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = 7728af0633a8d9d119c9b36a5c5a441e11e75ff1
PKG_LICENSE = CC-0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
