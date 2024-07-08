PKG_SOURCE_LOCAL=~/work/riot-tee
PKG_NAME=riot-tee
PKG_URL=https://github.com/Einhornhool/riot-tee
PKG_VERSION=4b79ef4d339ee789d9520ca46c128be46592454e
PKG_LICENSE=GPL-v3.0

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.10.0

export DEBUG=1
export NRFXPATH = $(RIOTBASE)/build/pkg/nrfx
export CMSISPATH = $(RIOTBASE)/build/pkg/cmsis/CMSIS
export BUILDDIR = $(PKG_BUILD_DIR)
export RIOT_BUILD=1

TEE_CFLAGS += -Wno-strict-prototypes
TEE_CFLAGS += -Wno-old-style-definition

TOOLCHAIN_FILE=$(PKG_SOURCE_DIR)/xcompile-toolchain.cmake

all: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)
	mkdir $(BINDIR)/riot-tee
	cp $(PKG_BUILD_DIR)/nsc_lib.o $(BINDIR)/riot-tee

$(PKG_BUILD_DIR)/Makefile: $(TOOLCHAIN_FILE)
	cmake \
		-DCMAKE_C_FLAGS=$(TEE_CFLAGS) \
		-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		-B$(PKG_BUILD_DIR) \
		-H$(PKG_SOURCE_DIR)

$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

git-download: | ..cmake_version_supported

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

clean::
	@rm -rf $(RIOTBASE)/build/pkg/riot-tee
	@rm -rf $(BINDIR)
