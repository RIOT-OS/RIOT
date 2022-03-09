PKG_NAME=elf2uf2
PKG_URL=https://github.com/raspberrypi/pico-sdk.git
PKG_VERSION=2062372d203b372849d573f252cf7c6dc2800c0a
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

all: $(CURDIR)/elf2uf2

$(CURDIR)/elf2uf2:
	@env -i PATH="$(PATH)" cmake $(PKG_SOURCE_DIR)/tools/elf2uf2/CMakeLists.txt
	"$(MAKE)" -C "$(PKG_SOURCE_DIR)/tools/elf2uf2"
	cp $(PKG_SOURCE_DIR)/tools/elf2uf2/elf2uf2 .
