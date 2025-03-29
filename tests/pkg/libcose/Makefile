include ../Makefile.pkg_common

TEST_ON_CI_WHITELIST += native32 native64

USEPKG += libcose
# By default we use hacl as crypto backend, uncomment to use a different
# crypto backend.
USEMODULE += libcose_crypt_hacl
# USEMODULE += libcose_crypt_c25519
USEMODULE += memarray
USEMODULE += embunit

ifneq (,$(filter libcose_crypt_monocypher,$(USEMODULE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(6*THREAD_STACKSIZE_DEFAULT\)
else
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)
endif

include $(RIOTBASE)/Makefile.include
