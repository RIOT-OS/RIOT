PKG_NAME = cryptoservice
PKG_URL = https://github.com/janthies/CryptoService.git
PKG_VERSION = 9b0f00e7863a79322bd5d198ff70bbe2f9b779be
PKG_LICENSE = LGPL-2.1-only

include $(RIOTBASE)/pkg/pkg.mk

FLAGS += -Wno-cast-align

ifneq (,$(filter trustzone_m_nonsecure,$(USEMODULE)))
  FLAGS += -DTOOLCHAINPATH=/usr/bin
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
