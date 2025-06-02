PKG_NAME=esp32_sdk
PKG_URL=https://github.com/espressif/esp-idf
# v5.4
PKG_VERSION=c8bb53292d08d6449a09823cf554e62ac839cd8c
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

ESP32_SDK_VER_FILE = $(PKG_SOURCE_DIR)/components/esp_idf_ver.h
ESP32_SDK_VER_CMD = $(shell git -C $(PKG_SOURCE_DIR) describe --tags $(PKG_VERSION))

all: $(ESP32_SDK_VER_FILE)

$(PKG_PREPARED): $(ESP32_SDK_VER_FILE)

# Set the SDK version from the SDK hash/tag. For example "v4.4-98-geb3797dc3ff".
$(ESP32_SDK_VER_FILE): $(PKG_PATCHED)
	$(Q)echo "#define IDF_VER \"$(ESP32_SDK_VER_CMD)\"" > $@
