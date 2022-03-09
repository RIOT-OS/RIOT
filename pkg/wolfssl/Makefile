PKG_NAME=wolfssl
PKG_URL=https://github.com/wolfssl/wolfssl.git
PKG_VERSION=0fa5af9929ce2ee99e8789996a3048f41a99830e # v4.5.0
PKG_LICENSE=GPLv2

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
