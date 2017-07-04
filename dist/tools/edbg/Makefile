PKG_NAME=edbg
PKG_URL=https://github.com/ataradov/edbg
PKG_VERSION=935bb4b1efd0e2f8434cf97443e0d95b9e02ec1d
PKG_LICENSE=BSD-3-Clause
PKG_BUILDDIR=$(CURDIR)/bin

# CC and CXX variables are set by RIOT's build process to cross compiling for
# a specific target platfrom. Thus, we have to unset them to build edgb using
# the system C/C++ compilers - i.e, to avoid conflicts on macOS systems
CC=
CXX=

.PHONY: all

all: git-download
	"$(MAKE)" -C $(PKG_BUILDDIR)
	mv $(PKG_BUILDDIR)/edbg .

include $(RIOTBASE)/pkg/pkg.mk
