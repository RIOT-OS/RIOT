PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = d3d88e2d8e8da53d278aabe448cb338dbf845f09
PKG_LICENSE = CC-0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
