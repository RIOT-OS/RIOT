PKG_NAME=esp32_sdk_libs
PKG_URL=https://github.com/espressif/esp32-wifi-lib
# This is a version in the v3.1 beta1 release branch
PKG_VERSION=534a9b14101af90231d40a4f94924d67bc848d5f
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

ESP32_SDK_DIR = $(PKGDIRBASE)/esp32_sdk

# Directory where we place the libraries and headers.
ESP32_SDK_BUILD_DIR = $(ESP32_SDK_DIR)/build-libs

# We need to place all binary libraries shipped with the SDK in a common
# directory.
ESP32_SDK_COMPONENT_LIBS = \
    libcoexist.a \
    libcore.a \
    libespnow.a \
    libmesh.a \
    libnet80211.a \
    libphy.a \
    libpp.a \
    librtc.a \
    libsmartconfig.a \
    libwpa.a \
    libwpa2.a \
    libwps.a

ESP32_SDK_LIBS = $(addprefix $(ESP32_SDK_BUILD_DIR)/, $(ESP32_SDK_COMPONENT_LIBS))

all: $(ESP32_SDK_LIBS)

$(PKG_PREPARED): $(ESP32_SDK_BUILD_DIR)

$(ESP32_SDK_BUILD_DIR): $(PKG_PATCHED)
	$(Q)mkdir -p $(ESP32_SDK_BUILD_DIR)

$(ESP32_SDK_BUILD_DIR)/lib%.a: \
    $(PKG_SOURCE_DIR)/lib%.a | $(ESP32_SDK_BUILD_DIR)
	$(Q)cp $? $(ESP32_SDK_BUILD_DIR)
