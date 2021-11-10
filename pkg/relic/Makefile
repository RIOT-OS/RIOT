PKG_NAME=relic
PKG_URL=https://github.com/relic-toolkit/relic.git
PKG_VERSION=e3f82e36250b228f3b4646781f6e06a8be497d18
PKG_LICENSE=LGPL-2.1

PKG_CUSTOM_PREPARED = $(BINDIR)/pkg-build/$(PKG_NAME)/include

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: ..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS += -Wno-unused-function -Wno-old-style-definition -Wno-unused-parameter \
          -Wno-pedantic -Wno-array-parameter -Wno-unused-but-set-variable \
          -Wno-stringop-overflow

TOOLCHAIN_FILE = $(PKG_SOURCE_DIR)/xcompile-toolchain.cmake

all: $(BINDIR)/$(PKG_NAME).a

$(PKG_CUSTOM_PREPARED):
	$(info [INFO] Relic: create include directory in build)
	mkdir -p $(PKG_CUSTOM_PREPARED)

$(BINDIR)/$(PKG_NAME).a: $(PKG_BUILD_DIR)/lib/librelic_s.a
	cp $< $@

$(PKG_BUILD_DIR)/lib/librelic_s.a: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)

# Pass 'COMP' with a target specific export to not have issues with the shell
# escaping evaluation.
COMP = $(filter-out -Werror -Werror=old-style-definition -Werror=strict-prototypes -std=gnu11,$(CFLAGS))
$(PKG_BUILD_DIR)/Makefile: export COMP ?=
$(PKG_BUILD_DIR)/Makefile: $(PKG_PREPARED) $(TOOLCHAIN_FILE) | ..cmake_version_supported
	cmake -B$(PKG_BUILD_DIR) -H$(PKG_SOURCE_DIR) \
			-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
			-DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off -Wno-dev $(RELIC_CONFIG_FLAGS)

$(TOOLCHAIN_FILE): FORCE
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake
