PKG_NAME=cryptoauthlib
PKG_URL=https://github.com/MicrochipTech/cryptoauthlib
PKG_VERSION=055dd4afafb019db1f4d61880aa441832139faa2
PKG_LICENSE=LGPL-2.1
PKG_TEST_NAME=cryptoauthlib_test

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: all..cmake_version_supported

CMAKE_MINIMAL_VERSION = 3.6.0

CFLAGS += $(INCLUDES) # for some reason this is needed by llvm, but not for gcc
CFLAGS += -DATCA_HAL_I2C
CFLAGS += -Wno-cast-align
CFLAGS += -Wno-char-subscripts
CFLAGS += -Wno-overflow
CFLAGS += -Wno-pointer-to-int-cast
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-strict-aliasing
CFLAGS += -Wno-type-limits
CFLAGS += -Wno-unused-function
CFLAGS += -Wno-unused-parameter
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-maybe-uninitialized

TOOLCHAIN_FILE=$(PKG_SOURCE_DIR)/xcompile-toolchain.cmake

ifneq (,$(filter $(PKG_TEST_NAME),$(USEMODULE)))
    all: cryptoauth build_tests
else
    all: cryptoauth
endif

cryptoauth: $(PKG_BUILD_DIR)/Makefile
	$(MAKE) -C $(PKG_BUILD_DIR)
	cp $(PKG_BUILD_DIR)/libcryptoauth.a $(BINDIR)/$(PKG_NAME).a

# TODO: Drop `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` once upstream is compatible
#       with newer cmake (e.g. compilation still succeeds with cmake 4.x)
$(PKG_BUILD_DIR)/Makefile: $(TOOLCHAIN_FILE)
	cmake -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN_FILE) \
		  -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
		  -Wno-dev \
		  -DATCA_NO_HEAP:BOOL=TRUE \
		  -B$(PKG_BUILD_DIR) \
		  -H$(PKG_SOURCE_DIR)/lib

$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

build_tests:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/third_party/unity -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_third_party_unity
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/jwt -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_jwt
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/tng -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_tng
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/atcacert -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_atcacert
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/api_atcab -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_api_atcab
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/api_calib -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_api_calib
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/api_crypto -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_api_crypto
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/test/vectors -f $(RIOTBASE)/Makefile.base MODULE=$(PKG_TEST_NAME)_vectors

git-download: | ..cmake_version_supported

..cmake_version_supported:
	@ # Remove '-rcX' from version as they are not well handled
	$(Q)\
	CMAKE_VERSION=$$(cmake --version | sed -n '1 {s/cmake version //;s/-rc.*//;p;}'); \
	$(RIOTTOOLS)/has_minimal_version/has_minimal_version.sh "$${CMAKE_VERSION}" "$(CMAKE_MINIMAL_VERSION)" cmake

clean::
	@rm -rf $(BINDIR)/$(PKG_NAME).a
	@rm -rf $(BINDIR)/$(PKG_TEST_NAME).a
