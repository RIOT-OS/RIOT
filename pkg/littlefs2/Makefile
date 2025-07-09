PKG_NAME=littlefs2
PKG_URL=https://github.com/littlefs-project/littlefs.git
# v2.11.1
PKG_VERSION=8e251dd675da00342d45dac78b6f627f119aed03
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-format

# replace pkg supplied logging macros with RIOT's
CFLAGS += -include $(PKG_DIR)/lfs_log.h

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
