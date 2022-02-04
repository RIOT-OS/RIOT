PKG_NAME=light_ws2812
PKG_URL=https://github.com/cpldcpu/light_ws2812.git
PKG_VERSION=cd149996012fe96bc3d7883cb18a0103fd8e8b3a
PKG_LICENSE=GPL-3

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/light_ws2812_ARM/ -f $(CURDIR)/$(PKG_NAME).mk
