PKG_NAME=nanopb
PKG_URL=https://github.com/nanopb/nanopb.git
PKG_VERSION=be0dd4b3aacdc9c2bbe21568b6a37218f2bb959a
PKG_LICENSE=zlib

.PHONY: all

RIOT_CFLAGS = $(INCLUDES) -std=c99

TOOLCHAIN_FILE = $(PKG_BUILDDIR)/xcompile-toolchain.cmake

PKG_DIR=$(CURDIR)
PROTO_DIR=$(APPDIR)/proto
PROTO_BUILD_DIR=$(APPDIR)/proto_compiled
PROTO_FILES=$(shell ls $(PROTO_DIR))

all: $(PKG_BUILDDIR)/Makefile
	cp $(PKG_BUILDDIR)/libnanopb.a $(BINDIR)/nanopb.a

$(PKG_BUILDDIR)/Makefile: $(TOOLCHAIN_FILE)
	rm -rf $(PROTO_BUILD_DIR) && \
	mkdir $(PROTO_BUILD_DIR) && \
	cp $(PKG_DIR)/Makefile.template $(PKG_BUILDDIR)/Makefile && \
	$(MAKE) -e PROTO_BUILD_DIR="$(PROTO_BUILD_DIR)" -e PROTO_FILES="$(PROTO_FILES)" \
	 -e PROTO_DIR="$(PROTO_DIR)" -e PKG_BUILDDIR="$(PKG_BUILDDIR)" -C $(PKG_BUILDDIR) proto lib

$(TOOLCHAIN_FILE): git-download
	$(RIOTTOOLS)/cmake/generate-xcompile-toolchain.sh > $(TOOLCHAIN_FILE)

include $(RIOTBASE)/pkg/pkg.mk
