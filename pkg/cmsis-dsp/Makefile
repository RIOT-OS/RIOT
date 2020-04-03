PKG_NAME=cmsis-dsp
PKG_URL=https://github.com/ARM-software/CMSIS_5
PKG_VERSION=5.4.0
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

CFLAGS += -Wno-strict-aliasing -Wno-unused-parameter

all:
	"$(MAKE)" -C $(PKG_BUILDDIR) -f $(CURDIR)/Makefile.$(PKG_NAME) all
