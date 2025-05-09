include ../Makefile.sys_common

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

USEMODULE += psa_hash
USEMODULE += psa_hash_sha_256
USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_p256r1
USEMODULE += psa_asymmetric_ecc_p256r1_custom_backend
USEMODULE += psa_asymmetric_ecc_p256r1_backend_microecc

CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1

include $(RIOTBASE)/Makefile.include

ifneq (,$(filter psa_asymmetric_ecc_p256r1_backend_microecc,$(USEMODULE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096
endif

ifneq (,$(filter psa_asymmetric_ecc_p256r1_backend_periph,$(USEMODULE)))
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=7000
endif
