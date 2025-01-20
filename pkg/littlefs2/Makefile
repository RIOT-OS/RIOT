PKG_NAME=littlefs2
PKG_URL=https://github.com/littlefs-project/littlefs.git
# v2.10.1
PKG_VERSION=366100b1403d2b680ed7a0f3bd0ba982c34d5c07
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# replace pkg supplied logging macros with RIOT's
CFLAGS += -include $(PKG_DIR)/lfs_log.h

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
