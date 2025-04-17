PKG_NAME=pioasm
PKG_URL=https://github.com/raspberrypi/pico-sdk.git
# release SDK 1.5.0
PKG_VERSION=2ccab115de0d42d31d6611cca19ef0cd0d2ccaa7
PKG_LICENSE=BSD-3-Clause

RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(RIOTBASE)/dist/tools

PKG_PATCH_DIR=$(RIOTTOOLS)/pioasm/patches
PKG_BUILD_DIR=$(PKG_SOURCE_DIR)/tools/pioasm

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/pioasm

$(CURDIR)/pioasm: $(PKG_PREPARED)
	@env -i PATH="$(PATH)" cmake $(PKG_SOURCE_DIR)/tools/pioasm/CMakeLists.txt -B"$(PKG_BUILD_DIR)"
	"$(MAKE)" -C "$(PKG_SOURCE_DIR)/tools/pioasm"
	cp $(PKG_BUILD_DIR)/pioasm .
