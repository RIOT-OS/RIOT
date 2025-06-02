PKG_NAME=driver_sx126x
PKG_URL=https://github.com/Lora-net/sx126x_driver
# v2.3.2
PKG_VERSION=9636dc4660ada4eeddf91eb7b3f7f241000bf202
PKG_LICENSE=BSD

CFLAGS += -Wno-unused-parameter

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/$(PKG_NAME).mk
