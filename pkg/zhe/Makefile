PKG_NAME=		zhe
PKG_URL=		https://github.com/atolab/zhe
PKG_VERSION=	5d7844b1560f0c3c2369f330c11c5b5827bc1f93


.PHONY: all


all: git-download
	mkdir -p "$(PKG_BUILDDIR)/build"
	cd "$(PKG_BUILDDIR)/build" && cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_C_FLAGS="-Wno-strict-prototypes -m32"
#~ 	cd "$(PKG_BUILDDIR)/build" && cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_TOOLCHAIN_FILE=~/Downloads/gcc/cross_compile.cmake
	cd "$(PKG_BUILDDIR)/build" && make
	cp -a $(PKG_BUILDDIR)/build/src/CMakeFiles/zhe.dir/*.o $(PKG_BUILDDIR)
	cp -a $(PKG_BUILDDIR)/build/src/CMakeFiles/zhe.dir/__/example/platform/*.o $(PKG_BUILDDIR)
	cd "$(PKG_BUILDDIR)" && ar r $(BINDIR)/zhe.a *.o
	rm -Rf $(PKG_BUILDDIR)/*.o
	
include $(RIOTBASE)/pkg/pkg.mk
