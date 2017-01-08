PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=0b1de2da1ef407ee5793c0e7eda420391ae056dc
PKG_LICENSE=ISC

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download
	"$(MAKE)" -BC $(PKG_BUILDDIR)/src lib-ccn-lite.a
	"$(MAKE)" -BC $(PKG_BUILDDIR)/src lib-ccn-lite-utils.a
	cp $(PKG_BUILDDIR)/src/lib-ccn-lite.a ${BINDIR}/ccn-lite.a
	cp $(PKG_BUILDDIR)/src/lib-ccn-lite-utils.a ${BINDIR}/ccn-lite-utils.a

include $(RIOTBASE)/pkg/pkg.mk
