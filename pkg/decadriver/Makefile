PKG_NAME=decadriver
PKG_URL=https://github.com/br101/dw3000-decadriver-source
# Commit from September 18th 2026
PKG_VERSION=67dfbb7f2c5b1a4157b8c265b19f08776e91b1cc
# The code by br101 is licensed under ISC while the original Qorvo code has a
# custom license from Qorvo. The license is attached to the sources.
PKG_LICENSE=ISC AND LicenseRef-QORVO-2

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/Makefile.$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/decadriver/platform
