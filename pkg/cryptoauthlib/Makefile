PKG_NAME=cryptoauthlib
PKG_URL=https://github.com/MicrochipTech/cryptoauthlib
PKG_VERSION=af8187776cd3f3faf8bed412eaf6ff7221862e19
PKG_LICENSE=LGPL-2.1
PKG_TEST_NAME=cryptoauthlib_test
PKG_TESTINCLDIR = $(PKG_BUILDDIR)/test

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS += $(INCLUDES)
CFLAGS += -Wno-missing-field-initializers -Wno-unused-function
CFLAGS += -Wno-type-limits -Wno-strict-aliasing -Wno-unused-variable -DATCA_HAL_I2C
CFLAGS += -Wno-unused-parameter -Wno-sign-compare -Wno-overflow -Wno-pointer-to-int-cast
CFLAGS += -Wno-char-subscripts

TOOLCHAIN_FILE=$(PKG_BUILDDIR)/xcompile-toolchain.cmake

ifneq (,$(filter $(PKG_TEST_NAME),$(USEMODULE)))
    all: cryptoauth build_tests
else
    all: cryptoauth
endif

cryptoauth: $(PKG_BUILDDIR)/lib/Makefile
	$(MAKE) -C $(PKG_BUILDDIR)/lib
	cp $(PKG_BUILDDIR)/lib/libcryptoauth.a $(BINDIR)/$(PKG_NAME).a

$(PKG_BUILDDIR)/lib/Makefile: $(TOOLCHAIN_FILE)
	cd $(PKG_BUILDDIR)/lib && \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -Wno-dev \
		  -DBUILD_TESTS=OFF \
		  -DATCA_HAL_I2C:BOOL=TRUE .

$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

build_tests:
	cp $(PKG_TESTINCLDIR)/*.c $(PKG_BUILDDIR)
	cp $(PKG_TESTINCLDIR)/jwt/*.c $(PKG_BUILDDIR)
	cp $(PKG_TESTINCLDIR)/tng/*.c $(PKG_BUILDDIR)
	cp $(PKG_TESTINCLDIR)/atcacert/*.c $(PKG_BUILDDIR)

	echo "MODULE = $(PKG_TEST_NAME)" > $(PKG_BUILDDIR)/Makefile
	echo "include \$$(RIOTBASE)/Makefile.base" >> $(PKG_BUILDDIR)/Makefile

	"$(MAKE)" -C $(PKG_BUILDDIR)

git-download: | ..cmake_version_supported

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a
	@rm -rf $(BINDIR)/$(PKG_TEST_NAME).a
