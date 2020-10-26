PKG_NAME=gecko_sdk
PKG_URL=https://github.com/basilfx/RIOT-gecko-sdk
PKG_VERSION=b47581a678a53c01dacea9a0a3956c67854b12f4
PKG_LICENSE=Zlib

include $(RIOTBASE)/pkg/pkg.mk

ifneq ($(CPU),efm32)
  $(error This package can only be used with EFM32 CPUs)
endif

ifneq (llvm,$(TOOLCHAIN))
  CFLAGS += -Wno-int-in-bool-context
endif

all:
	"$(MAKE)" -C $(PKG_SOURCE_DIR)/dist
