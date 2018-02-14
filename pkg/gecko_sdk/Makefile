PKG_NAME=gecko_sdk
PKG_URL=https://github.com/basilfx/RIOT-gecko-sdk
PKG_VERSION=d6a90efb97f175101821304e6e5a1ba20eef02aa
PKG_LICENSE=Zlib

ifneq ($(CPU),efm32)
  $(error This package can only be used with EFM32 CPUs)
endif

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)/dist

include $(RIOTBASE)/pkg/pkg.mk
