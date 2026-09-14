PKG_NAME=picosdk
PKG_URL=https://github.com/raspberrypi/pico-sdk.git
# Version 2.2.0
PKG_VERSION=a1438dff1d38bd9c65dbd693f0e5db4b9ae91779
PKG_LICENSE=BSD-3-Clause

include $(RIOTBASE)/pkg/pkg.mk

# nothing to build for "USEPKG += picosdk"
all:

# pioasm is part of the Pico SDK and the pioasm target is called when
# the PIO peripheral is used by an application
#
# Note: The `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` flag should be fixed upstream
#       removed once the upstream SDK has been patched.
pioasm: $(PKG_PREPARED)
	@env -i PATH="$(PATH)" cmake $(PKG_SOURCE_DIR)/tools/pioasm/CMakeLists.txt \
	                           -B"$(PKG_BUILD_DIR)/tools/pioasm" \
	                           -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
	                           -DPIOASM_VERSION_STRING=0.1 \
	                           -DPIOASM_EXTRA_SOURCE_FILES=$(RIOTPKG)/picosdk/contrib/riot_output.cpp
	"$(MAKE)" -C "$(PKG_BUILD_DIR)/tools/pioasm"
	cp $(PKG_BUILD_DIR)/tools/pioasm/pioasm .

clean::
	@rm -rf $(RIOTPKG)/picosdk/pioasm
