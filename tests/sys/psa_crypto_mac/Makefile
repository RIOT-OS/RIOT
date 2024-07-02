include ../Makefile.sys_common

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

USEMODULE += psa_mac
USEMODULE += psa_mac_hmac_sha_256

CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1

include $(RIOTBASE)/Makefile.include
