PKG_NAME=gecko_sdk
PKG_URL=https://github.com/basilfx/RIOT-gecko-sdk
PKG_VERSION=0a7cb308308adbb3f65f84c538971149b0c040b4
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
