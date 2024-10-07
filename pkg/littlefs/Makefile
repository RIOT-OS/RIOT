PKG_NAME=littlefs
PKG_URL=https://github.com/ARMmbed/littlefs.git
# v1.7.2
PKG_VERSION=7e110b44c0e796dc56e2fe86587762d685653029
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# replace pkg supplied logging macros with RIOT's
CFLAGS += -include $(PKG_DIR)/lfs_log.h

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
