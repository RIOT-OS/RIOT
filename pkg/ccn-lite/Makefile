PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=74568442b48b1154719fce8210d9974d75537502
PKG_LICENSE=ISC

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: ..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

RIOT_CFLAGS = $(INCLUDES)

ifeq (llvm,$(TOOLCHAIN))
  RIOT_CFLAGS += -Wno-char-subscripts
endif

TOOLCHAIN_FILE = $(PKG_SOURCE_DIR)/xcompile-toolchain.cmake

CFLAGS += -Wno-cast-align

all: $(BINDIR)/ccn-lite.a

$(BINDIR)/ccn-lite.a: $(PKG_BUILD_DIR)/lib/libccnl-riot.a
	cp $< $@

$(PKG_BUILD_DIR)/lib/libccnl-riot.a: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)

$(PKG_BUILD_DIR)/Makefile: $(PKG_PREPARED) $(TOOLCHAIN_FILE) | ..cmake_version_supported
	cmake -B$(PKG_BUILD_DIR) -H$(PKG_SOURCE_DIR)/src \
	      -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
	      -DCCNL_RIOT=1 -DRIOT_CFLAGS="$(RIOT_CFLAGS)" -DBUILD_TESTING=OFF

$(TOOLCHAIN_FILE): FORCE
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

ifneq (,$(filter -Wformat-nonliteral -Wformat=2, $(CFLAGS)))
  CFLAGS += -Wno-format-nonliteral
endif
