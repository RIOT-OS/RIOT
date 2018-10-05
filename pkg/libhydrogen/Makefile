PKG_NAME    = libhydrogen
PKG_URL     = https://github.com/jedisct1/libhydrogen
PKG_VERSION = 39eb529905ce118b674a7723c0d2b48074b9986d
PKG_LICENSE = ISC

# This warning is triggered on non-32bit platforms
CFLAGS += -Wno-type-limits

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR) \
			  -f $(RIOTPKG)/libhydrogen/Makefile.$(PKG_NAME)

include $(RIOTBASE)/pkg/pkg.mk
