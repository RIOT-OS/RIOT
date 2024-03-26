PKG_NAME=micropython
PKG_URL=https://github.com/kaspar030/micropython
PKG_VERSION=4ff7897215b9f8d65e147935949e88cb7cc28652
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
