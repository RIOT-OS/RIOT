include ../Makefile.sys_common

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

USEMODULE += psa_cipher
USEMODULE += psa_cipher_aes_128_cbc

CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1

include $(RIOTBASE)/Makefile.include
