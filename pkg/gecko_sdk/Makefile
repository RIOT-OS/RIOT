PKG_NAME=gecko_sdk
PKG_URL=https://github.com/basilfx/RIOT-gecko-sdk
PKG_VERSION=91ebaf05d173ba0a9ba86b6e5078f6159ba8bc2a
PKG_LICENSE=Zlib

RIOTBASE ?= ../../

include $(RIOTBASE)/makefiles/utils/strings.mk

PKG_SPARSE_PATHS += dist/Makefile

PKG_SPARSE_PATHS += dist/platform/common
PKG_SPARSE_PATHS += dist/platform/Device/SiliconLabs/$(call uppercase_and_underscore,$(CPU_FAM))
PKG_SPARSE_PATHS += dist/platform/emlib-extra
PKG_SPARSE_PATHS += dist/platform/emlib

ifneq (,$(filter gecko_sdk_librail,$(USEMODULE)))
	PKG_SPARSE_PATHS += dist/platform/radio
endif

include $(RIOTBASE)/pkg/pkg.mk

ifneq ($(CPU),efm32)
  $(error This package can only be used with EFM32 CPUs)
endif

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/dist
