include ../Makefile.sys_common

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_ed25519

ifneq (1, $(SHOULD_RUN_KCONFIG))
  CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
  CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1
endif

CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096

include $(RIOTBASE)/Makefile.include
