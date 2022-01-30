PKG_NAME=esp32_sdk
PKG_URL=https://github.com/espressif/esp-idf
# This is a version in the v3.1 beta1 release branch
PKG_VERSION=f198339ec09e90666150672884535802304d23ec
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

# This directory is shared across all apps since there's no need to replicate
# these libraries on each app.
ESP32_SDK_BUILD_DIR = $(PKG_SOURCE_DIR)/build-libs
ESP32_SDK_VER_FILE = $(PKG_SOURCE_DIR)/components/esp32_idf_version.h
ESP32_SDK_VER_CMD = $(shell git -C $(PKG_SOURCE_DIR) describe --tags $(PKG_VERSION))

# We need to place all binary libraries shipped with the SDK in a common
# directory.
ESP32_SDK_COMPONENT_LIBS = libhal.a

ESP32_SDK_LIBS = $(addprefix $(ESP32_SDK_BUILD_DIR)/, $(ESP32_SDK_COMPONENT_LIBS))

all: $(ESP32_SDK_LIBS) $(ESP32_SDK_VER_FILE)

$(PKG_PREPARED): $(ESP32_SDK_BUILD_DIR) $(ESP32_SDK_VER_FILE)

$(ESP32_SDK_BUILD_DIR): $(PKG_PATCHED)
	$(Q)mkdir -p $(ESP32_SDK_BUILD_DIR)

# Set the SDK version from the SDK hash/tag. For example "v3.1-51-g913a06a9".
$(ESP32_SDK_VER_FILE): $(PKG_PATCHED) | $(ESP32_SDK_BUILD_DIR)
	$(Q)echo "#define IDF_VER \"$(ESP32_SDK_VER_CMD)\"" > $@

$(ESP32_SDK_BUILD_DIR)/lib%.a: \
    $(PKG_SOURCE_DIR)/components/esp32/lib%.a | $(ESP32_SDK_BUILD_DIR)
	$(Q)cp $? $(ESP32_SDK_BUILD_DIR)
