PKG_NAME=ccn-lite
PKG_URL=https://github.com/OlegHahm/ccn-lite/
PKG_VERSION=39b1406c11de9de364220909488eebabe7e81613

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: download
	"$(MAKE)" -BC $(PKG_BUILDDIR)/src lib-ccn-lite.a
	"$(MAKE)" -BC $(PKG_BUILDDIR)/src lib-ccn-lite-utils.a
	cp $(PKG_BUILDDIR)/src/lib-ccn-lite.a ${BINDIR}/ccn-lite.a
	cp $(PKG_BUILDDIR)/src/lib-ccn-lite-utils.a ${BINDIR}/ccn-lite-utils.a

include $(RIOTBASE)/pkg/pkg.mk
