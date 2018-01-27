PKG_NAME=edbg
PKG_URL=https://github.com/ataradov/edbg
PKG_VERSION=76f85abdea212ba23760723cce15e00ca4ae4b76
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR=$(CURDIR)/bin

.PHONY: all

all: git-download
# Start edbg build in a clean environment, so variables set by RIOT's build process
# for cross compiling a specific target platfrom are reset and edbg can
# be built cleanly for the native platform.
	env -i PATH=$(PATH) TERM=$(TERM) "$(MAKE)" -C $(PKG_BUILDDIR)
	mv $(PKG_BUILDDIR)/edbg .

include $(RIOTBASE)/pkg/pkg.mk
