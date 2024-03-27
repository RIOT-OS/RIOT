# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

APPLICATION = example_psa_crypto

BOARD ?= native

ifeq (2, $(SECURE_ELEMENT))
  CFLAGS += -DSECURE_ELEMENT # Application specific (not needed by PSA)
  CFLAGS += -DMULTIPLE_SE # Application specific (not needed by PSA)
  CFLAGS += -DCUSTOM_ATCA_PARAMS # Application specific (not needed by PSA)
  INCLUDES += -I$(APPDIR)
  KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.multi_se
else
  KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.base
endif

USEMODULE += ztimer
USEMODULE += ztimer_usec

USEMODULE += psa_crypto

# Hashes are needed for ECDSA operations (including secure elements), which
# is why we always build them
USEMODULE += psa_hash
USEMODULE += psa_hash_sha_256

ifeq (1, $(SECURE_ELEMENT))
  # When using a secure element, the type is required.
  # Also you can specify the number of key slots required to store keys.
  CFLAGS += -DSECURE_ELEMENT # Application specific (not needed by PSA)
  CFLAGS += -DCUSTOM_ATCA_PARAMS  # Application specific (not needed by PSA)
  INCLUDES += -I$(APPDIR) # Application specific (not needed by PSA)
  ifneq (1, $(SHOULD_RUN_KCONFIG))
    CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=4
    CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1
  else
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.se
  endif
  USEMODULE += psa_secure_element
  USEMODULE += psa_secure_element_ateccx08a
  USEMODULE += psa_secure_element_ateccx08a_cipher_aes_128
  USEMODULE += psa_secure_element_ateccx08a_hmac_sha256
  USEMODULE += psa_secure_element_ateccx08a_ecc_p256
else ifeq (2, $(SECURE_ELEMENT))
  CFLAGS += -DSECURE_ELEMENT # Application specific (not needed by PSA)
  CFLAGS += -DMULTIPLE_SE # Application specific (not needed by PSA)
  CFLAGS += -DCUSTOM_ATCA_PARAMS # Application specific (not needed by PSA)
  INCLUDES += -I$(APPDIR) # Application specific (not needed by PSA)
  ifneq (1, $(SHOULD_RUN_KCONFIG))
    CFLAGS += -DCONFIG_PSA_MAX_SE_COUNT=2
    CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=8
    CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=2
  else
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.multi_se
  endif
  USEMODULE += psa_secure_element
  USEMODULE += psa_secure_element_multiple
  USEMODULE += psa_secure_element_ateccx08a
  USEMODULE += psa_secure_element_ateccx08a_cipher_aes_128
  USEMODULE += psa_secure_element_ateccx08a_hmac_sha256
  USEMODULE += psa_secure_element_ateccx08a_ecc_p256
else ifdef CUSTOM_BACKEND
  # Necessary configuration when using Make dependency resolution
  # This first part chooses the operation. If nothing else is specified,
  # a default backend is built depending on the platform capabilities.
  USEMODULE += psa_cipher
  USEMODULE += psa_cipher_aes_128_cbc

  USEMODULE += psa_mac
  USEMODULE += psa_mac_hmac_sha_256

  USEMODULE += psa_asymmetric
  USEMODULE += psa_asymmetric_ecc_p256r1
  USEMODULE += psa_asymmetric_ecc_ed25519

  # If you want to use a custom backend, you need to do it this way.
  USEMODULE += psa_cipher_aes_128_cbc_custom_backend
  USEMODULE += psa_cipher_aes_128_cbc_backend_riot # force custom backend

  USEMODULE += psa_mac_hmac_sha_256_custom_backend
  USEMODULE += psa_mac_hmac_sha_256_backend_riot # force custom backend

  USEMODULE += psa_hash_sha_256_custom_backend
  USEMODULE += psa_hash_sha_256_backend_riot

  USEMODULE += psa_asymmetric_ecc_p256r1_custom_backend
  USEMODULE += psa_asymmetric_ecc_p256r1_backend_microecc # force custom backend

  USEMODULE += psa_asymmetric_ecc_ed25519_custom_backend
  USEMODULE += psa_asymmetric_ecc_ed25519_backend_c25519 # force custom backend
else
  # Necessary configuration when using Make dependency resolution
  # This part only chooses the operation. If nothing else es specified,
  # a default backend is built depending on the platform capabilities.
  USEMODULE += psa_cipher
  USEMODULE += psa_cipher_aes_128_cbc

  USEMODULE += psa_mac
  USEMODULE += psa_mac_hmac_sha_256

  USEMODULE += psa_asymmetric
  USEMODULE += psa_asymmetric_ecc_p256r1
  USEMODULE += psa_asymmetric_ecc_ed25519

endif

ifndef SECURE_ELEMENT
  # The software implementations need a larger stack, so we increase the stack size.
  CFLAGS += -DTHREAD_STACKSIZE_MAIN=\(12*THREAD_STACKSIZE_DEFAULT\)
  ifneq (1, $(SHOULD_RUN_KCONFIG))
    CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=2
    CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=4
  else
    KCONFIG_ADD_CONFIG += $(APPDIR)/app.config.base
  endif
endif

include $(RIOTBASE)/Makefile.include
