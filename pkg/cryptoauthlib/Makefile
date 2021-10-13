PKG_NAME=cryptoauthlib
PKG_URL=https://github.com/MicrochipTech/cryptoauthlib
PKG_VERSION=af8187776cd3f3faf8bed412eaf6ff7221862e19
PKG_LICENSE=LGPL-2.1
PKG_TEST_NAME=cryptoauthlib_test

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS += $(INCLUDES)
CFLAGS += -Wno-missing-field-initializers -Wno-unused-function
CFLAGS += -Wno-type-limits -Wno-strict-aliasing -Wno-unused-variable -DATCA_HAL_I2C
CFLAGS += -Wno-unused-parameter -Wno-sign-compare -Wno-overflow -Wno-pointer-to-int-cast
CFLAGS += -Wno-char-subscripts

TOOLCHAIN_FILE=$(PKG_SOURCE_DIR)/xcompile-toolchain.cmake

ifneq (,$(filter $(PKG_TEST_NAME),$(USEMODULE)))
    all: cryptoauth build_tests
else
    all: cryptoauth
endif

cryptoauth: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)
	cp $(PKG_BUILD_DIR)/libcryptoauth.a $(BINDIR)/$(PKG_NAME).a

$(PKG_BUILD_DIR)/Makefile: $(TOOLCHAIN_FILE)
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -Wno-dev \
		  -DATCA_HAL_I2C:BOOL=TRUE \
		  -B$(PKG_BUILD_DIR) \
		  -H$(PKG_SOURCE_DIR)/lib

$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

build_tests:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/jwt -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_jwt
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/tng -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_tng
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/atcacert -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_atcacert

git-download: | ..cmake_version_supported

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a
	@rm -rf $(BINDIR)/$(PKG_TEST_NAME).a
