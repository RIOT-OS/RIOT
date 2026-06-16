PKG_SOURCE_LOCAL ?= $(wildcard $(HOME)/work/riot-tee)
PKG_NAME=riot-tee
PKG_URL=git@github.com:janthies/riot-tee.git
PKG_VERSION=ac0fbeb72c926089aa60c7fcc6fe177476590aaf
PKG_LICENSE=GPL-v3.0

include $(RIOTBASE)/pkg/pkg.mk
include $(RIOTBASE)/makefiles/riot-tee-layout.mk

.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.30.0

TEE_CFLAGS += -Wno-strict-prototypes
TEE_CFLAGS += -Wno-old-style-definition
TEE_CFLAGS += -DTOOLCHAINPATH=/usr/bin
TEE_CFLAGS += -DRIOT_CRYPTOSERVICE=$(RIOTBASE)/build/pkg/cryptoservice

TOOLCHAIN_FILE=$(PKG_BUILD_DIR)/xcompile-toolchain.cmake

all: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(BINDIR)/riot-tee
	cp $(PKG_BUILD_DIR)/nsc_lib.o $(BINDIR)/riot-tee

LAYOUT_MK := $(RIOTBASE)/makefiles/riot-tee-layout.mk

# tf-m must be prepared before cmake configures: riot-tee's CMakeLists lists the
# cc3xx driver sources (under build/pkg/tf-m) as add_library sources, so they
# have to exist in build/pkg/tf-m before this configure step runs.
$(PKG_BUILD_DIR)/Makefile: $(TOOLCHAIN_FILE) $(LAYOUT_MK)
	"$(MAKE)" -C $(RIOTBASE)/pkg/tf-m/
	@rm -rf $(PKG_BUILD_DIR)/CMakeCache.txt
	git -C $(PKG_SOURCE_DIR) submodule update --init --recursive
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_C_FLAGS=$(TEE_CFLAGS) \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DTEE_FLASH_START=$(TEE_FLASH_START) \
		-DTEE_FLASH_SIZE=$(TEE_FLASH_SIZE) \
		-DTEE_RAM_SIZE=$(TEE_RAM_SIZE) \
		-DFIRST_NS_FLASH_REGION=$(FIRST_NS_FLASH_REGION) \
		-DFIRST_NS_RAM_REGION=$(FIRST_NS_RAM_REGION) \
		-DNSC_FLASH_REGION=$(NSC_FLASH_REGION) \
		-DTFM_PATH=$(RIOTBASE)/build/pkg/tf-m \
		-B$(PKG_BUILD_DIR) \
		-H$(PKG_SOURCE_DIR)

$(TOOLCHAIN_FILE): | ..cmake_version_supported
	mkdir -p $(PKG_BUILD_DIR)
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

clean::
	@rm -rf $(RIOTBASE)/build/pkg/riot-tee
	@rm -rf $(BINDIR)
