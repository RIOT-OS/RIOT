PKG_NAME=relic
PKG_URL=https://github.com/relic-toolkit/relic.git
PKG_VERSION=0b0442a8218df8d309266923f2dd5b9ae3b318ce
PKG_LICENSE=LGPL-2.1

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: ..cmake_version_supported

RELIC_BUILD_DIR = $(PKG_BUILDDIR)/build
RELIC_SOURCE_DIR = $(PKG_BUILDDIR)

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS += -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-function -Wno-newline-eof

TOOLCHAIN_FILE = $(RELIC_SOURCE_DIR)/xcompile-toolchain.cmake

all: $(BINDIR)/$(PKG_NAME).a

$(BINDIR)/$(PKG_NAME).a: $(RELIC_BUILD_DIR)/lib/librelic_s.a
	cp $< $@

$(RELIC_BUILD_DIR)/lib/librelic_s.a: $(RELIC_BUILD_DIR)/Makefile
	$(MAKE) -C $(RELIC_BUILD_DIR)

# Pass 'COMP' with a target specific export to not have issues with the shell
# escaping evaluation.
COMP = $(filter-out -Werror -Werror=old-style-definition -Werror=strict-prototypes -std=gnu99,$(CFLAGS))
$(RELIC_BUILD_DIR)/Makefile: export COMP ?=
$(RELIC_BUILD_DIR)/Makefile: $(PKG_PREPARED) $(TOOLCHAIN_FILE) | ..cmake_version_supported
	cmake -B$(RELIC_BUILD_DIR) -H$(RELIC_SOURCE_DIR) \
			-DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
			-DCHECK=off -DTESTS=0 -DBENCH=0 -DSHLIB=off -Wno-dev $(RELIC_CONFIG_FLAGS)

$(TOOLCHAIN_FILE): FORCE
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake
