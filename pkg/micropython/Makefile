PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=0e9a7e5d67873125cceb0d02faaf40c80730ac33
PKG_LICENSE=MIT

CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter -Wno-error

# Micropython build system doesn't support (yet) out-of-source builds
# so clone and build micropython within the application build directory
PKG_BUILD_OUT_OF_SOURCE = 0

include $(RIOTBASE)/pkg/pkg.mk

# Note: We have to explicitly override CC, as otherwise micropython will
# set its own CC.
all:
	@mkdir -p $(PKG_BUILD_DIR)/tmp
	BUILD=$(PKG_BUILD_DIR) "$(MAKE)" CC=$(CC) -C $(PKG_SOURCE_DIR)/ports/riot
