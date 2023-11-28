PKG_NAME=littlefs2
PKG_URL=https://github.com/littlefs-project/littlefs.git
# v2.8.1
PKG_VERSION=c733d9ec5776dfc949ec6dc71fa9ce3ff71de6e5
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# Disable debug printing
ifneq ($(DEVELHELP),1)
  CFLAGS += -DLFS_NO_DEBUG -DLFS_NO_WARN -DLFS_NO_ERROR
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
