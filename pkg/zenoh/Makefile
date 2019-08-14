PKG_NAME=		zenoh
PKG_URL=		https://github.com/atolab/zenoh-c
PKG_VERSION=		b2f6cfb72cb41fd68551840207fc7fa28ef5871d


.PHONY: all


all: git-download
	mkdir -p "$(PKG_BUILDDIR)/build"
	cd "$(PKG_BUILDDIR)/build" && cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_C_FLAGS="-Wno-strict-prototypes -m32"
	cd "$(PKG_BUILDDIR)/build" && make 
	
	cp -a $(PKG_BUILDDIR)/build/CMakeFiles/zenohc.dir/src/*.o $(PKG_BUILDDIR)
	cp -a $(PKG_BUILDDIR)/build/CMakeFiles/zenohc.dir/src/unix/*.o $(PKG_BUILDDIR)

	cd "$(PKG_BUILDDIR)" && ar r $(BINDIR)/zenoh.a *.o
	rm -Rf $(PKG_BUILDDIR)/*.o
	
include $(RIOTBASE)/pkg/pkg.mk
