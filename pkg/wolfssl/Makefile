PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
PKG_VERSION=3f13b49fa318fbd3216d7da36d942e7c276d3413 # v4.3.0
PKG_LICENSE=GPLv2

include $(RIOTBASE)/pkg/pkg.mk

all:  # Nothing to do here when building

prepare:
	cp Makefile.wolfssl $(PKG_BUILDDIR)/src/Makefile
	cp Makefile.wolfcrypt $(PKG_BUILDDIR)/wolfcrypt/src/Makefile
	touch $(PKG_BUILDDIR)/wolfssl.a
	cp Makefile.wolfcrypt-test $(PKG_BUILDDIR)/wolfcrypt/test/Makefile
	cp Makefile.wolfcrypt-benchmark $(PKG_BUILDDIR)/wolfcrypt/benchmark/Makefile
