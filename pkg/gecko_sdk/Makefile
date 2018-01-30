PKG_NAME=gecko_sdk
PKG_URL=https://github.com/basilfx/RIOT-gecko-sdk
PKG_VERSION=f8a90e4db4680984b5f9d3dd44ca528772458456
PKG_LICENSE=Zlib

ifneq ($(CPU),efm32)
  $(error This package can only be used with EFM32 CPUs)
endif

CFLAGS += -Wno-error=implicit-fallthrough

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/dist

include $(RIOTBASE)/pkg/pkg.mk
