PKG_NAME=gecko_sdk
PKG_URL=https://github.com/basilfx/RIOT-gecko-sdk
PKG_VERSION=a5e7be18ef3d7947ab6f04f9cd74f8808da7aece
PKG_LICENSE=Zlib

ifneq ($(CPU),efm32)
  $(error This package can only be used with EFM32 CPUs)
endif

CFLAGS += -Wno-int-in-bool-context

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/dist

include $(RIOTBASE)/pkg/pkg.mk
