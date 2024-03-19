PKG_NAME    = libhydrogen
PKG_URL     = https://github.com/jedisct1/libhydrogen
PKG_VERSION = 0d4224321eb2b3224fc4bf9bc65e3331054fc6ae
PKG_LICENSE = ISC

include $(RIOTBASE)/pkg/pkg.mk

# This warning is triggered on non-32bit platforms
CFLAGS += -Wno-type-limits
CFLAGS += -Wno-cast-align

# Silence warning popping up since `gcc version 11.2.0 (Ubuntu 11.2.0-19ubuntu1)`
CFLAGS += -Wno-stringop-overflow

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
