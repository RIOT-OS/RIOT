PKG_NAME=micropython
PKG_URL=https://github.com/micropython/micropython
# v1.29.0
PKG_VERSION=0fd6c573ea815774668bbb16b8e197c8822368b2
PKG_LICENSE=MIT

include $(RIOTBASE)/pkg/pkg.mk

# MicroPython's build system does not integrate well with RIOT, so we just call
# it directly from the Makefile. We have to explicitly override CC, otherwise
# MicroPython will set its own CC.
all:
	$(QQ)"$(MAKE)" -C $(RIOTBASE)/pkg/micropython/contrib CC=$(CC) TOP=$(PKG_SOURCE_DIR) BUILD=$(PKG_BUILD_DIR)
