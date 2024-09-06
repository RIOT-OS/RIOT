# PKG_SOURCE_LOCAL=~/work/CryptoService
PKG_NAME = cryptoservice
PKG_URL = git@github.com:LP-HAW/CryptoService.git
# PKG_VERSION = bd22261a3bde77c4865621165137f7d43446a89b
PKG_VERSION = feature/nrf9160dk-target-integration
PKG_LICENSE = unknown

include $(RIOTBASE)/pkg/pkg.mk

# all:
.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.30.0

FLAGS += -Wno-cast-align

ifneq (,$(filter nrf9160dk-ns,$(BOARD)))
  FLAGS += -DTOOLCHAINPATH=/home/lena/apps/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi/bin
  FLAGS += -DCMAKE_TOOLCHAIN_FILE=cmake/armGNU.cmake
endif

all: $(PKG_BUILD_DIR)/Makefile

# Configuration is needed to make micro-ecc dependency available
$(PKG_BUILD_DIR)/Makefile:
	cmake \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS=$(FLAGS) \
		-B$(PKG_BUILD_DIR) \
		-H$(PKG_SOURCE_DIR)

clean::
	@rm -rf $(RIOTBASE)/build/pkg/riot-tee
	@rm -rf $(BINDIR)
