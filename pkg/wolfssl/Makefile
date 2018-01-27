PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
#PKG_VERSION=a79f9c93c98f1af4e6b2c70abe8208bab8919944
#PKG_VERSION=f7cd8a0f15774f8fdb686c13008ec48601b64b28
PKG_VERSION=cc4ca6a44aebcbbb7f7073e963d95198fd931465
PKG_LICENSE=GPLv2

.PHONY: all

export RIOT_CFLAGS = ${CFLAGS} ${INCLUDES}

all: git-download
	mkdir $(PKG_BUILDDIR)/../../../$(BOARD)/wolfssl/
	mkdir $(PKG_BUILDDIR)/../../../$(BOARD)/wolfssl/src
	mkdir $(PKG_BUILDDIR)/../../../$(BOARD)/wolfssl/wolfcrypt
	mkdir $(PKG_BUILDDIR)/../../../$(BOARD)/wolfssl/wolfcrypt/src
	cp Makefile.wolfssl $(PKG_BUILDDIR)/Makefile
	"$(MAKE)" -C $(PKG_BUILDDIR)/

include $(RIOTBASE)/pkg/pkg.mk


