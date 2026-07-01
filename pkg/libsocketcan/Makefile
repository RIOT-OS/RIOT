PKG_NAME=libsocketcan
PKG_URL=https://git.pengutronix.de/git/tools/libsocketcan
PKG_VERSION=077def398ad303043d73339112968e5112d8d7c8
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-cast-align

# HACK: Ignore `posix_headers` module here: libsocketcan needs to use the
#       host's OS's socket implementation rather than the RIOT one, as we
#       rely on the host's CAN implementation to provide CAN on native
INCLUDES := $(filter-out -I%/sys/posix/include,$(INCLUDES))

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.$(PKG_NAME)
