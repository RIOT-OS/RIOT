include ../Makefile.sys_common

USEMODULE += ztimer
USEMODULE += ztimer_usec
USEMODULE += psa_crypto

USEMODULE += psa_aead
USEMODULE += psa_aead_aes_128_ccm

CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=2

include $(RIOTBASE)/Makefile.include
