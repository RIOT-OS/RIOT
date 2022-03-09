PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = b2b1d26e4144753c8e91d005c8a4079c0e77439f
PKG_LICENSE = CC-0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
