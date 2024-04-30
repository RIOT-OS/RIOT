include ../Makefile.sys_common

USEMODULE += embunit

USEMODULE += psa_crypto

CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(8*THREAD_STACKSIZE_DEFAULT\)

CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1

USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_ed25519

include $(RIOTBASE)/Makefile.include
