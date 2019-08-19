PKG_NAME=		zenoh_cc2538_wip
PKG_URL=		https://github.com/atolab/zenoh-c
PKG_VERSION=	b2f6cfb72cb41fd68551840207fc7fa28ef5871d


.PHONY: all


all: git-download
	mkdir -p "$(PKG_BUILDDIR)/build"
	cd "$(PKG_BUILDDIR)/build" && cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_TOOLCHAIN_FILE=$(RIOTBASE)/examples/zenoh_cc2538_wip/cross_compile.cmake -DCMAKE_C_FLAGS="-Wno-strict-prototypes"	
	cd "$(PKG_BUILDDIR)/build" && make 
	cp "$(PKG_BUILDDIR)/build/libzenohc.a" $(BINDIR)/zenoh_cc2538_wip.a

include $(RIOTBASE)/pkg/pkg.mk
