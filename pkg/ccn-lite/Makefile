PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=d2ea61ff450d0f3636aa29af3e448ef10ee95f9e
PKG_LICENSE=ISC

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download
	cd $(PKG_BUILDDIR)/src && cmake -DCCNL_RIOT=1 -DRIOT_CFLAGS="${RIOT_CFLAGS}" . && make
	cp $(PKG_BUILDDIR)/src/lib/libccnl-riot.a ${BINDIR}/ccn-lite.a

include $(RIOTBASE)/pkg/pkg.mk
