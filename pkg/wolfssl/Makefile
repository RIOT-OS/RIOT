PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
PKG_VERSION=0fa5af9929ce2ee99e8789996a3048f41a99830e # v4.5.0
PKG_LICENSE=GPLv2

include $(RIOTBASE)/pkg/pkg.mk

.PHONY: wolfcrypt%

all: $(filter wolfcrypt wolfcrypt-test wolfcrypt-benchmark,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.wolfssl

wolfcrypt:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfcrypt/src -f $(CURDIR)/Makefile.wolfcrypt

wolfcrypt-%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfcrypt/$* -f $(CURDIR)/Makefile.wolfcrypt-$*
