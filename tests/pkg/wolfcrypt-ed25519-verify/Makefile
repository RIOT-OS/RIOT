include ../Makefile.pkg_common

# Ensure there is enough stack space
CFLAGS += -DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_LARGE

CFLAGS+=-DNO_ED25519_SIGN -DNO_ED25519_KEY_EXPORT

USEPKG +=wolfssl
USEMODULE += wolfcrypt
USEMODULE += wolfcrypt_ed25519
USEMODULE += wolfcrypt_random

include $(RIOTBASE)/Makefile.include
