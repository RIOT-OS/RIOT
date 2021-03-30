PKG_NAME=talking_leds
PKG_URL=https://github.com/fabriziop/TalkingLED
PKG_VERSION=8ae4f2d0b736aa338f24e097dbaf876fbb385dbd
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
