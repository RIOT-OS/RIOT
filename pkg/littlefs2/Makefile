PKG_NAME=littlefs2
PKG_URL=https://github.com/littlefs-project/littlefs.git
# v2.11.2
PKG_VERSION=adad0fbbcf5382c20978d07f94f9c13be9041c1b
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# replace pkg supplied logging macros with RIOT's
CFLAGS += -include $(PKG_DIR)/lfs_log.h

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
