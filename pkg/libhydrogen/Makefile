PKG_NAME    = libhydrogen
PKG_URL     = https://github.com/jedisct1/libhydrogen
PKG_VERSION = a4de6e098b5bbbd5389eb8058130f782b53826c3
PKG_LICENSE = ISC

include $(RIOTBASE)/pkg/pkg.mk

# This warning is triggered on non-32bit platforms
CFLAGS += -Wno-type-limits

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
