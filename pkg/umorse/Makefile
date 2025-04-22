PKG_NAME=umorse
PKG_URL=https://github.com/smlng/uMorse
PKG_VERSION=ad941aa786d8ae4371016a30c4ca8ed5ccb39e3e
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -D_XOPEN_SOURCE=600

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(RIOTBASE)/Makefile.base
