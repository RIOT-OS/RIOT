PKG_NAME=littlefs2
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v2.5.1
PKG_VERSION=6a53d76e90af33f0656333c1db09bd337fa75d23
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# Disable debug printing
ifneq ($(DEVELHELP),1)
  CFLAGS += -DLFS_NO_DEBUG -DLFS_NO_WARN -DLFS_NO_ERROR
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
