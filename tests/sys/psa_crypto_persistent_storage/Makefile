include ../Makefile.sys_common

BOARD_WHITELIST = \
  native \
  nrf52840dk \
  #

USEMODULE += embunit

USEMODULE += psa_crypto
USEMODULE += psa_persistent_storage

USEMODULE += psa_cipher
USEMODULE += psa_cipher_aes_128_cbc

USEMODULE += psa_asymmetric
USEMODULE += psa_asymmetric_ecc_ed25519

CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1
CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(4*THREAD_STACKSIZE_DEFAULT\)

include $(RIOTBASE)/Makefile.include
