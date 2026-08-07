PKG_NAME=micropython
PKG_URL=https://github.com/micropython/micropython
# v1.28.0
PKG_VERSION=e0e9fbb17ed6fd06bb76e266ae554784c9c80804
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# MicroPython's build system does not integrate well with RIOT-OS, so we just
# call it directly from the Makefile. We have to explicitly override CC, as
# otherwise MicroPython will set its own CC.
all:
	@mkdir -p $(PKG_BUILD_DIR)/tmp
	$(QQ)"$(MAKE)" -C $(RIOTBASE)/pkg/micropython/contrib CC=$(CC) TOP=$(PKG_SOURCE_DIR) BUILD=$(PKG_BUILD_DIR)
