include ../Makefile.sys_common

USEMODULE += embunit

# This configuration also builds the backend implementations for all those selected
# modules, some of which depend on the PSA hash functions. So to make this test
# compile, we add the psa_hash module.
USEMODULE += psa_hash
USEMODULE += psa_hash_sha_256

USEMODULE += psa_crypto
USEMODULE += psa_persistent_storage

USEMODULE += psa_cipher
USEMODULE += psa_cipher_aes_128_cbc

USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_p256r1

CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1
CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=1

include $(RIOTBASE)/Makefile.include
