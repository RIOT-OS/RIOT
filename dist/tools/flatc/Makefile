PKG_NAME=flatc
PKG_URL=https://github.com/google/flatbuffers
PKG_VERSION=9e7e8cbe9f675123dd41b7c62868acad39188cae
PKG_LICENSE=Apache2.0

PKG_BUILDDIR=$(CURDIR)/bin

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

include $(RIOTBASE)/pkg/pkg.mk

all:
	@cd "$(PKG_BUILDDIR)" && env -i PATH="$(PATH)" cmake -G "Unix Makefiles" $(CMAKE_OPTIONS) .
	"$(MAKE)" -C "$(PKG_BUILDDIR)"
	@mv "$(PKG_BUILDDIR)"/flatc $(CURDIR)/flatc

distclean::
	@rm -f $(CURDIR)/flatc
