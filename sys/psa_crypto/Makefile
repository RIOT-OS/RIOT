
MODULE_DEFS_NEEDED += \
  psa_aead \
  psa_aead_aes_128_ccm \
  psa_aead_aes_192_ccm \
  psa_aead_aes_256_ccm \
  psa_asymmetric \
  psa_asymmetric_ecc_ed25519 \
  psa_asymmetric_ecc_p192r1 \
  psa_asymmetric_ecc_p256r1 \
  psa_cipher \
  psa_cipher_aes_128_cbc \
  psa_cipher_aes_192_cbc \
  psa_cipher_aes_256_cbc \
  psa_cipher_chacha20 \
  psa_hash \
  psa_hash_md5 \
  psa_hash_sha3_256 \
  psa_hash_sha3_384 \
  psa_hash_sha3_512 \
  psa_hash_sha_1 \
  psa_hash_sha_224 \
  psa_hash_sha_256 \
  psa_hash_sha_384 \
  psa_hash_sha_512 \
  psa_hash_sha_512_224 \
  psa_hash_sha_512_256 \
  psa_key_agreement \
  psa_key_derivation \
  psa_key_management \
  psa_key_slot_mgmt \
  psa_mac \
  psa_mac_hmac_md5 \
  psa_mac_hmac_sha3_256 \
  psa_mac_hmac_sha3_384 \
  psa_mac_hmac_sha3_512 \
  psa_mac_hmac_sha_1 \
  psa_mac_hmac_sha_224 \
  psa_mac_hmac_sha_256 \
  psa_mac_hmac_sha_384 \
  psa_mac_hmac_sha_512 \
  psa_persistent_storage \
  psa_secure_element \
  #
# PSA Crypto casts like hell, so this needs to shut the f up
CFLAGS += -Wno-cast-align

# include the PSA headers
INCLUDES += -I$(RIOTBASE)/sys/psa_crypto/include

ifneq (,$(filter psa_key_slot_mgmt,$(USEMODULE)))
  DIRS += psa_key_slot_mgmt
endif

ifneq (,$(filter psa_persistent_storage,$(USEMODULE)))
  DIRS += psa_persistent_storage
endif

ifneq (,$(filter psa_se_mgmt,$(USEMODULE)))
  DIRS += psa_se_mgmt
endif

include $(RIOTBASE)/Makefile.base
