PKG_NAME=		zhe
PKG_URL=		https://github.com/atolab/zhe
PKG_VERSION=	5d7844b1560f0c3c2369f330c11c5b5827bc1f93


.PHONY: all


all: git-download
	mkdir -p "$(PKG_BUILDDIR)/build"
	cd "$(PKG_BUILDDIR)/build" && cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_C_FLAGS="-Wno-strict-prototypes -m32"
	cd "$(PKG_BUILDDIR)/build" && make
	cp $(PKG_BUILDDIR)/build/src/libzhe.a $(BINDIR)/zhe.a
include $(RIOTBASE)/pkg/pkg.mk
