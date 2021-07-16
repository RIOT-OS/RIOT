PKG_NAME=esp8266_sdk
PKG_URL=https://github.com/espressif/ESP8266_RTOS_SDK
# This is a version in the v3.1 release branch, between 3.1 and 3.1.1.
PKG_VERSION=913a06a9ac3b2f18009e8fee8f092ca9ffeccd38
PKG_LICENSE=Apache-2.0

include $(RIOTBASE)/pkg/pkg.mk

# This directory is shared across all apps since there's no need to replicate
# these libraries on each app.
ESP8266_SDK_BUILD_DIR = $(PKG_SOURCE_DIR)/build-libs

# We need to replace some symbols in the binary libraries shipped with the SDK
# to avoid collisions with RIOT symbols.
ESP_SDK_COMPONENT_LIBS = \
  libcore.a libespnow.a libgcc.a libhal.a libnet80211.a libphy.a libpp.a \
  libsmartconfig.a libssc.a libwpa.a libwps.a

ESP_SDK_LIBS = $(addprefix $(ESP8266_SDK_BUILD_DIR)/, $(ESP_SDK_COMPONENT_LIBS))

all: $(ESP_SDK_LIBS) $(ESP8266_SDK_BUILD_DIR)/esp8266_idf_version.h

$(ESP8266_SDK_BUILD_DIR):
	$(Q)mkdir -p $(ESP8266_SDK_BUILD_DIR)

# Set the SDK version from the SDK hash/tag. For example "v3.1-51-g913a06a9".
$(ESP8266_SDK_BUILD_DIR)/esp8266_idf_version.h: | $(ESP8266_SDK_BUILD_DIR)
	$(Q)echo "#define IDF_VER \"$(shell git -C $(PKG_SOURCE_DIR) describe --tags)\"" \
	  > $@

$(ESP8266_SDK_BUILD_DIR)/lib%.a: \
    $(PKG_SOURCE_DIR)/components/esp8266/lib/lib%.a | $(ESP8266_SDK_BUILD_DIR)
	$(Q)$(OBJCOPY) \
	  --redefine-syms $(CURDIR)/symbol_renames.txt \
	  --redefine-sym printf=$(patsubst lib%.a,%,$(notdir $@))_printf \
	  --redefine-sym ets_printf=$(patsubst lib%.a,%,$(notdir $@))_ets_printf \
	  $< $@
