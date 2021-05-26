PKG_NAME    = nanocbor
PKG_URL     = https://github.com/bergzand/nanocbor
PKG_VERSION = 57c48edaca2b5792a41119610035335761dd3c81
PKG_LICENSE = CC-0

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
