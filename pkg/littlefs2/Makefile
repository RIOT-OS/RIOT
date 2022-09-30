PKG_NAME=littlefs2
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.5.0
PKG_VERSION=40dba4a556e0d81dfbe64301a6aa4e18ceca896c
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# Disable debug printing
ifneq ($(DEVELHELP),1)
  CFLAGS += -DLFS_NO_DEBUG -DLFS_NO_WARN -DLFS_NO_ERROR
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
