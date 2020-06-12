PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=2da4123b055d90c406c08cf2e96704e132220536
PKG_LICENSE=ISC

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: ..cmake_version_supported

CCNLITE_SOURCE_DIR = $(PKG_BUILDDIR)
CCNLITE_BUILD_DIR = $(PKG_BUILDDIR)/build

CMAKE_MINIMAL_VERSION = 3.6.0

RIOT_CFLAGS = $(INCLUDES)

ifeq (llvm,$(TOOLCHAIN))
  RIOT_CFLAGS += -Wno-char-subscripts
endif

TOOLCHAIN_FILE = $(CCNLITE_SOURCE_DIR)/xcompile-toolchain.cmake

all: $(BINDIR)/ccn-lite.a

$(BINDIR)/ccn-lite.a: $(CCNLITE_BUILD_DIR)/lib/libccnl-riot.a
	cp $< $@

$(CCNLITE_BUILD_DIR)/lib/libccnl-riot.a: $(CCNLITE_BUILD_DIR)/Makefile
	$(MAKE) -C $(CCNLITE_BUILD_DIR)

$(CCNLITE_BUILD_DIR)/Makefile: $(PKG_PREPARED) $(TOOLCHAIN_FILE) | ..cmake_version_supported
	cmake -B$(CCNLITE_BUILD_DIR) -H$(CCNLITE_SOURCE_DIR)/src \
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
