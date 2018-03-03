PKG_NAME=ccn-lite
PKG_URL=https://github.com/cn-uofbasel/ccn-lite/
PKG_VERSION=8b17f2b43f337242dce3175efd95f5439cf4a4f5
PKG_LICENSE=ISC

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download
	cd $(PKG_BUILDDIR)/src && cmake -DCCNL_RIOT=1 -DRIOT_CFLAGS="${RIOT_CFLAGS}" . && make
	cp $(PKG_BUILDDIR)/src/lib/libccnl-riot.a ${BINDIR}/ccn-lite.a

include $(RIOTBASE)/pkg/pkg.mk
