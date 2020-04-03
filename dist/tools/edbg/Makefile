PKG_NAME=edbg
PKG_URL=https://github.com/ataradov/edbg
PKG_VERSION=99d15460fcff723f73b16c29c8ca14bff4b33b20
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR=$(CURDIR)/bin

include $(RIOTBASE)/pkg/pkg.mk

all:
# Start edbg build in a clean environment, so variables set by RIOT's build process
# for cross compiling a specific target platform are reset and edbg can
# be built cleanly for the native platform.
	env -i PATH="$(PATH)" TERM="$(TERM)" "$(MAKE)" -C "$(PKG_BUILDDIR)"
	mv $(PKG_BUILDDIR)/edbg .
