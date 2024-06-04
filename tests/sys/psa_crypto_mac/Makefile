include ../Makefile.sys_common

USEMODULE += embunit

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto
USEMODULE += psa_mac
USEMODULE += psa_hash

USEMODULE += psa_mac_hmac_sha_256
USEMODULE += psa_mac_hmac_sha_384
USEMODULE += psa_mac_hmac_sha_512

CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1

include $(RIOTBASE)/Makefile.include
