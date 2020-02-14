PKG_NAME=cmsis-nn
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=5.6.0
PKG_LICENSE=Apache-2.0
CFLAGS += -Wno-strict-aliasing -Wno-unused-parameter

include $(RIOTBASE)/pkg/pkg.mk

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME)
