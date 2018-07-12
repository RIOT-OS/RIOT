PKG_NAME=edbg
PKG_URL=https://github.com/ataradov/edbg
PKG_VERSION=a59b16ea18ea095135362035dcfd990ca1dd3af2
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR=$(CURDIR)/bin

.PHONY: all

all: git-download
# Start edbg build in a clean environment, so variables set by RIOT's build process
# for cross compiling a specific target platform are reset and edbg can
# be built cleanly for the native platform.
	env -i PATH=$(PATH) TERM=$(TERM) "$(MAKE)" -C $(PKG_BUILDDIR)
	mv $(PKG_BUILDDIR)/edbg .

include $(RIOTBASE)/pkg/pkg.mk
