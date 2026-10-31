PKG_NAME=riot-tee
PKG_URL=https://github.com/janthies/riot-tee.git
PKG_VERSION=8356f6e104885833ae0a3f9376509c37d166ebf5
PKG_LICENSE=LGPL-2.1-only

include $(RIOTBASE)/pkg/pkg.mk
include $(RIOTBASE)/pkg/riot-tee/riot-tee-layout.mk

TEE_CFLAGS += -Wno-strict-prototypes
TEE_CFLAGS += -Wno-old-style-definition

TOOLCHAIN_FILE=$(PKG_BUILD_DIR)/xcompile-toolchain.cmake

all: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir -p $(BINDIR)/riot-tee
	cp $(PKG_BUILD_DIR)/nsc_lib.o $(BINDIR)/riot-tee

LAYOUT_MK := $(RIOTBASE)/pkg/riot-tee/riot-tee-layout.mk

$(PKG_BUILD_DIR)/Makefile: $(TOOLCHAIN_FILE) $(LAYOUT_MK)
	@rm -rf $(PKG_BUILD_DIR)/CMakeCache.txt
	git -C $(PKG_SOURCE_DIR) submodule update --init --recursive
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-DCMAKE_C_FLAGS="$(TEE_CFLAGS)" \
		-DTOOLCHAINPATH=/usr/bin \
		-DRIOT_CRYPTOSERVICE=$(PKGDIRBASE)/cryptoservice \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-DTEE_FLASH_SIZE=$(TEE_FLASH_SIZE) \
		-DTEE_RAM_SIZE=$(TEE_RAM_SIZE) \
		-DTFM_PATH=$(PKGDIRBASE)/tf-m \
		-B$(PKG_BUILD_DIR) \
		-H$(PKG_SOURCE_DIR)

$(TOOLCHAIN_FILE):
	mkdir -p $(PKG_BUILD_DIR)
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)
