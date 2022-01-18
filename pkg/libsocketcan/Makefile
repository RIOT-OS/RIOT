PKG_NAME=libsocketcan
PKG_URL=https://git.pengutronix.de/git/tools/libsocketcan
PKG_VERSION=077def398ad303043d73339112968e5112d8d7c8
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)
