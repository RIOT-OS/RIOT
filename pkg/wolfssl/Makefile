PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
# Past v5.9.2
# https://github.com/wolfSSL/wolfssl/commit/addf0d4b743635a9e4c6d10d0c05458f724e56e3
# This commit hash was used because it enables off the line pinning for raw public keys
PKG_VERSION=addf0d4b743635a9e4c6d10d0c05458f724e56e3
PKG_LICENSE=GPL-3.0

include $(RIOTBASE)/pkg/pkg.mk

# some variable seem uninitialized to gcc with -Og but are not
# https://gcc.gnu.org/bugzilla/buglist.cgi?quicksearch=may%20be%20used%20uninitialized
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=42145
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90710#c1
CFLAGS += -Wno-maybe-uninitialized

# wolfcrypt uses uint8_t for block buffers, on which fast
# 32 bit operations are performed. From a quick peek at the
# code it looks like the buffers are correctly aligned, so
# this is a false positive.
CFLAGS += -Wno-cast-align

.PHONY: wolfcrypt%

all: $(filter wolfcrypt wolfcrypt-test wolfcrypt-benchmark,$(USEMODULE))
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/src -f $(CURDIR)/Makefile.wolfssl

wolfcrypt:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfcrypt/src -f $(CURDIR)/Makefile.wolfcrypt

wolfcrypt-%:
	$(QQ)"$(MAKE)" -C $(PKG_SOURCE_DIR)/wolfcrypt/$* -f $(CURDIR)/Makefile.wolfcrypt-$*
