PKG_NAME=driver_atwinc15x0
PKG_URL=https://github.com/arduino-libraries/WiFi101
# 1.6.1
PKG_VERSION=0cbee7f6a64a1260b8751d993b75c101ec3084c2
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-discarded-qualifiers
CFLAGS += -Wno-empty-body
CFLAGS += -Wno-old-style-definition
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-incompatible-pointer-types-discards-qualifiers

CFLAGS += -DETH_MODE
CFLAGS += -I$(PKG_SOURCE_DIR)/src

CFLAGS += -Wno-pedantic

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/driver/source -f $(RIOTBASE)/Makefile.base MODULE=driver_atwinc15x0
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src/common/source -f $(RIOTBASE)/Makefile.base MODULE=driver_atwinc15x0_common
