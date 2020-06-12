PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
PKG_VERSION=3f13b49fa318fbd3216d7da36d942e7c276d3413 # v4.3.0
PKG_LICENSE=GPLv2

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: wolfcrypt%

all: $(filter wolfcrypt wolfcrypt-test wolfcrypt-benchmark,$(USEMODULE))
	"$(MAKE)" -C $(PKG_BUILDDIR)/src -f $(CURDIR)/Makefile.wolfssl

wolfcrypt:
	"$(MAKE)" -C $(PKG_BUILDDIR)/wolfcrypt/src -f $(CURDIR)/Makefile.wolfcrypt

wolfcrypt-%:
	"$(MAKE)" -C $(PKG_BUILDDIR)/wolfcrypt/$* -f $(CURDIR)/Makefile.wolfcrypt-$*
