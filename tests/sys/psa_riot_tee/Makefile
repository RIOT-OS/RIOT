include ../Makefile.sys_common

BOARD_WHITELIST = \
  nrf9160dk-ns \
  #

USEMODULE += embunit

USEMODULE += psa_crypto
USEMODULE += psa_cryptoservice

USEMODULE += psa_hash
USEMODULE += psa_hash_sha_256
USEMODULE += psa_hash_sha_256_custom_backend
USEMODULE += psa_hash_sha_256_backend_cryptoservice

USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_p256r1
USEMODULE += psa_asymmetric_ecc_p256r1_custom_backend
USEMODULE += psa_asymmetric_ecc_p256r1_backend_cryptoservice

CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CFLAGS += -DCONFIG_PSA_SEALED_KEY_COUNT=1
CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1

CFLAGS += -DTHREAD_STACKSIZE_MAIN=7000

include $(RIOTBASE)/Makefile.include
