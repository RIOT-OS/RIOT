PKG_NAME=flatbuffers
PKG_URL=https://github.com/google/flatbuffers
PKG_VERSION=595bf0007ab1929570c7671f091313c8fc20644e
PKG_LICENSE=Apache2.0

# manually set some RIOT env vars, so this Makefile can be called stand-alone
RIOTBASE ?= $(CURDIR)/../../..
RIOTTOOLS ?= $(CURDIR)/..
PKG_SOURCE_DIR ?= $(CURDIR)/bin
PKG_BUILD_DIR ?= $(CURDIR)/bin/build

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
	@env -i PATH="$(PATH)" cmake -G "Unix Makefiles" $(CMAKE_OPTIONS) -H"$(PKG_SOURCE_DIR)" -B"$(PKG_BUILD_DIR)"
	"$(MAKE)" -C "$(PKG_BUILD_DIR)"
	@mv "$(PKG_BUILD_DIR)"/flatc $(CURDIR)/flatc

distclean::
	@rm -f $(CURDIR)/flatc
