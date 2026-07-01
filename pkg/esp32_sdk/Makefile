PKG_NAME=esp32_sdk
PKG_URL=https://github.com/espressif/esp-idf
# v5.4
PKG_VERSION=c8bb53292d08d6449a09823cf554e62ac839cd8c
PKG_LICENSE=Apache-2.0

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
