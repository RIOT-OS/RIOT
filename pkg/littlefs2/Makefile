PKG_NAME=littlefs2
PKG_URL=https://github.com/littlefs-project/littlefs.git
# v2.11.0
PKG_VERSION=16ceb6793449fa159f99aed4a766c2823f59cf3e
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# replace pkg supplied logging macros with RIOT's
CFLAGS += -include $(PKG_DIR)/lfs_log.h

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
