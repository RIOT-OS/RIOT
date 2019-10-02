PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git

# Current version: v4.1.0+
PKG_VERSION=eaeaaf12c11dd52ab0cd6833252ed559656e9826
PKG_LICENSE=GPLv2

include $(RIOTBASE)/pkg/pkg.mk

all:  # Nothing to do here when building

prepare:
	cp Makefile.wolfssl $(PKG_BUILDDIR)/src/Makefile
	cp Makefile.wolfcrypt $(PKG_BUILDDIR)/wolfcrypt/src/Makefile
	touch $(PKG_BUILDDIR)/wolfssl.a
	cp Makefile.wolfcrypt-test $(PKG_BUILDDIR)/wolfcrypt/test/Makefile
	cp Makefile.wolfcrypt-benchmark $(PKG_BUILDDIR)/wolfcrypt/benchmark/Makefile
