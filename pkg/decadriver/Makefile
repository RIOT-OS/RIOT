PKG_NAME=decadriver
PKG_URL=https://github.com/br101/dw3000-decadriver-source
# Commit from July 22nd 2026
PKG_VERSION=effcf6e601d20c323e8932575692bd7d565b8720
# The code by br101 is licensed under ISC while the original Qorvo code has a
# custom license from Qorvo. The license is attached to the sources.
PKG_LICENSE=ISC AND LicenseRef-QORVO-2

include $(RIOTBASE)/pkg/pkg.mk

all:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR) -f $(CURDIR)/Makefile.$(PKG_NAME)
	$(QQ)"$(MAKE)" -C $(RIOTPKG)/decadriver/platform
