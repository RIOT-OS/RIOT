PKG_NAME=adafruit_nrf52_bootloader
PKG_URL=https://github.com/adafruit/Adafruit_nRF52_Bootloader.git
# 0.9.2
PKG_VERSION=eefbe6bda98b80cb296e2f1bb1c982d58fe4c12d
PKG_LICENSE=MIT

PKG_CUSTOM_PREPARED = custom-prepare

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..
RIOTMAKE ?= $(RIOTBASE)/makefiles
PKGDIRBASE ?= $(BUILD_DIR)/pkg

ifeq (,$(BOARD))
  $(error BOARD not defined)
endif

ifeq (,$(PORT))
  $(error PORT not defined)
endif

ADA_BOARD_feather-nrf52840-sense := feather_nrf52840_sense
ADA_BOARD_feather-nrf52840 := feather_nrf52840_express
ADA_BOARD_seeedstudio-xiao-nrf52840 := xiao_nrf52840_ble

ADA_BOARD := $(ADA_BOARD_$(BOARD))

ifeq (,$(ADA_BOARD))
  $(error unsupported BOARD)
endif

include $(RIOTBASE)/pkg/pkg.mk

custom-prepare:
	@$(GIT_IN_PKG) submodule update --init

flash: $(PKG_PREPARED)
	"$(MAKE)" BOARD="$(ADA_BOARD)" SERIAL="$(PORT)" -C "$(PKG_SOURCE_DIR)" clean flash-dfu
