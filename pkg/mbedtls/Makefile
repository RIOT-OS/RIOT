PKG_NAME = mbedtls
PKG_URL = https://github.com/ARMmbed/mbedtls.git
PKG_VERSION = yotta-2.3.2

export MBEDTLS_ROOT=$(CURDIR)
INCLUDES+=-I$(MBEDTLS_ROOT)/include

.PHONY: all

all: git-download
	$(MAKE) -C $(PKG_BUILDDIR)/library
include $(RIOTBASE)/pkg/pkg.mk
