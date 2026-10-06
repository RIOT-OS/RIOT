PKG_NAME=esp32_sdk
PKG_URL=https://github.com/espressif/esp-idf
# v5.4
PKG_VERSION=c8bb53292d08d6449a09823cf554e62ac839cd8c
PKG_LICENSE=Apache-2.0

# Only check out the components that are actually used by RIOT. The full
# ESP-IDF checkout is ~240 MB, mostly examples, docs, tools and unused
# components, which slows down every build that has to clone the package.
PKG_SPARSE_PATHS += components/app_update
PKG_SPARSE_PATHS += components/bootloader
PKG_SPARSE_PATHS += components/bootloader_support
PKG_SPARSE_PATHS += components/bt/controller
PKG_SPARSE_PATHS += components/bt/include
PKG_SPARSE_PATHS += components/bt/porting
PKG_SPARSE_PATHS += components/driver
PKG_SPARSE_PATHS += components/efuse
PKG_SPARSE_PATHS += components/esp_adc
PKG_SPARSE_PATHS += components/esp_app_format
PKG_SPARSE_PATHS += components/esp_bootloader_format
PKG_SPARSE_PATHS += components/esp_coex
PKG_SPARSE_PATHS += components/esp_common
PKG_SPARSE_PATHS += components/esp_driver_gpio
PKG_SPARSE_PATHS += components/esp_driver_i2c
PKG_SPARSE_PATHS += components/esp_driver_rmt
PKG_SPARSE_PATHS += components/esp_driver_sdmmc
PKG_SPARSE_PATHS += components/esp_driver_spi
PKG_SPARSE_PATHS += components/esp_driver_uart
PKG_SPARSE_PATHS += components/esp_eth
PKG_SPARSE_PATHS += components/esp_event
PKG_SPARSE_PATHS += components/esp_hw_support
PKG_SPARSE_PATHS += components/esp_lcd
PKG_SPARSE_PATHS += components/esp_mm
PKG_SPARSE_PATHS += components/esp_netif
PKG_SPARSE_PATHS += components/esp_partition
PKG_SPARSE_PATHS += components/esp_phy
PKG_SPARSE_PATHS += components/esp_pm
PKG_SPARSE_PATHS += components/esp_psram
PKG_SPARSE_PATHS += components/esp_rom
PKG_SPARSE_PATHS += components/esp_system
PKG_SPARSE_PATHS += components/esp_timer
PKG_SPARSE_PATHS += components/esp_wifi
PKG_SPARSE_PATHS += components/freertos
PKG_SPARSE_PATHS += components/hal
PKG_SPARSE_PATHS += components/heap
PKG_SPARSE_PATHS += components/ieee802154
PKG_SPARSE_PATHS += components/log
PKG_SPARSE_PATHS += components/newlib
PKG_SPARSE_PATHS += components/nvs_flash
PKG_SPARSE_PATHS += components/riscv
PKG_SPARSE_PATHS += components/sdmmc
PKG_SPARSE_PATHS += components/soc
PKG_SPARSE_PATHS += components/spi_flash
PKG_SPARSE_PATHS += components/usb
PKG_SPARSE_PATHS += components/wpa_supplicant
PKG_SPARSE_PATHS += components/xtensa

include $(RIOTBASE)/pkg/pkg.mk

ESP32_SDK_VER_FILE = $(PKG_SOURCE_DIR)/components/esp_idf_ver.h

all: $(ESP32_SDK_VER_FILE)

$(PKG_PREPARED): $(ESP32_SDK_VER_FILE)

# Set the SDK version from the SDK hash/tag. For example "v4.4-98-geb3797dc3ff".
# When not using `git-clone`, a shallow repository will be created that
# lacks the commit history, from which the SDK version is derived.
# It therefore has to be unshallowed first.
$(ESP32_SDK_VER_FILE): $(PKG_PATCHED)
	$(Q)if $$(git -C $(PKG_SOURCE_DIR) rev-parse --is-shallow-repository); then \
	    git -C $(PKG_SOURCE_DIR) fetch $(GIT_QUIET) --no-recurse-submodules --unshallow; \
	fi; \
	echo "#define IDF_VER \"`git -C $(PKG_SOURCE_DIR) describe --tags $(PKG_VERSION)`\"" > $@
