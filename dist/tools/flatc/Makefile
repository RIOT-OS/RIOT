PKG_NAME=flatc
PKG_URL=https://github.com/google/flatbuffers
PKG_VERSION=9e7e8cbe9f675123dd41b7c62868acad39188cae
PKG_LICENSE=Apache2.0

PKG_BUILDDIR=$(CURDIR)/bin

FLATC_SOURCE_DIR = $(PKG_BUILDDIR)
FLATC_BUILD_DIR = $(PKG_BUILDDIR)/build

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..
GITCACHE ?= $(RIOTTOOLS)/git/git-cache

CMAKE_OPTIONS = \
    -DCMAKE_BUILD_TYPE=Release       \
    -DFLATBUFFERS_BUILD_TESTS=OFF    \
    -DFLATBUFFERS_BUILD_FLATHASH=OFF \
    -DFLATBUFFERS_BUILD_FLATLIB=OFF  \
    #

ifneq (, $(CCACHE))
    CMAKE_OPTIONS += -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
endif

include $(RIOTBASE)/pkg/pkg.mk

all:
	@env -i PATH="$(PATH)" cmake -G "Unix Makefiles" $(CMAKE_OPTIONS) -H"$(FLATC_SOURCE_DIR)" -B"$(FLATC_BUILD_DIR)"
	"$(MAKE)" -C "$(FLATC_BUILD_DIR)"
	@mv "$(FLATC_BUILD_DIR)"/flatc $(CURDIR)/flatc

distclean::
	@rm -f $(CURDIR)/flatc
