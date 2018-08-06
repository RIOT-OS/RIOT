PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=f85a2a055db92b5978f2d4f92b830b5b6b9acb42
PKG_LICENSE=ISC

.PHONY: all ..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0


RIOT_CFLAGS = $(INCLUDES)

TOOLCHAIN_FILE=$(PKG_BUILDDIR)/xcompile-toolchain.cmake

all: $(PKG_BUILDDIR)/src/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)/src && \
	cp $(PKG_BUILDDIR)/src/lib/libccnl-riot.a $(BINDIR)/ccn-lite.a

$(PKG_BUILDDIR)/src/Makefile: $(TOOLCHAIN_FILE)
	cd $(PKG_BUILDDIR)/src && \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -DCCNL_RIOT=1 -DRIOT_CFLAGS="$(RIOT_CFLAGS)" -DBUILD_TESTING=OFF .
$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

git-download: | ..cmake_version_supported

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake


include $(RIOTBASE)/pkg/pkg.mk
ifneq (,$(filter -Wformat-nonliteral -Wformat=2, $(CFLAGS)))
  CFLAGS += -Wno-format-nonliteral
endif
