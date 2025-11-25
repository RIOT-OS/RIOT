PKG_NAME=elf2uf2
PKG_URL=https://github.com/raspberrypi/pico-sdk.git
PKG_VERSION=2062372d203b372849d573f252cf7c6dc2800c0a
PKG_LICENSE=BSD-3-Clause
PKG_BUILD_DIR=$(PKG_SOURCE_DIR)/tools/elf2uf2

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/elf2uf2

$(CURDIR)/elf2uf2: $(PKG_PREPARED)
	@env -i PATH="$(PATH)" cmake $(PKG_SOURCE_DIR)/tools/elf2uf2/CMakeLists.txt -B"$(PKG_BUILD_DIR)"
	"$(MAKE)" -C "$(PKG_SOURCE_DIR)/tools/elf2uf2"
	cp $(PKG_BUILD_DIR)/elf2uf2 .
