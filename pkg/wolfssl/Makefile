PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
PKG_VERSION=a79f9c93c98f1af4e6b2c70abe8208bab8919944
PKG_LICENSE=GPLv2

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download
	mkdir $(PKG_BUILDDIR)/../../../native/wolfssl/
	mkdir $(PKG_BUILDDIR)/../../../native/wolfssl/src
	mkdir $(PKG_BUILDDIR)/../../../native/wolfssl/wolfcrypt
	mkdir $(PKG_BUILDDIR)/../../../native/wolfssl/wolfcrypt/src
	cp Makefile.wolfssl $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/

include $(RIOTBASE)/pkg/pkg.mk


