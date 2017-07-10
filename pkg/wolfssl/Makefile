PKG_NAME=wolfssl
PKG_URL=https://github.com/danielinux/wolfssl.git
#PKG_URL=https://github.com/wolfssl/wolfssl.git
PKG_VERSION=859475e30f95a8b12132341da7bb4a2a25869656
PKG_LICENSE=GPLv2

.PHONY: all
all:  # Nothing to do here when building

prepare:
	cp Makefile.wolfssl $(PKG_BUILDDIR)/src/Makefile
	cp Makefile.wolfcrypt $(PKG_BUILDDIR)/wolfcrypt/src/Makefile
	touch $(PKG_BUILDDIR)/wolfssl.a
	cp Makefile.wolfcrypt-test $(PKG_BUILDDIR)/wolfcrypt/test/Makefile
	cp Makefile.wolfcrypt-benchmark $(PKG_BUILDDIR)/wolfcrypt/benchmark/Makefile

include $(RIOTBASE)/pkg/pkg.mk
