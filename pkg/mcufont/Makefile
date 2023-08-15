PKG_NAME=mcufont
PKG_URL=https://github.com/mcufont/mcufont
PKG_VERSION=9f3aa41b231195e7b2b59f78d8f01d06460b6d35
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/decoder -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_NAME)
