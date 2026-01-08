#ifdef __cplusplus
extern "C" {
#endif

#include "psa_error.h"
#include "crys_chacha_poly.h"
#include "crys_chacha_poly_error.h"
#include "psa/crypto.h"
#include "cryptocell_310_util.h"

#define ENABLE_DEBUG 1
#include "debug.h"

psa_status_t psa_aead_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer, size_t key_buffer_length,
                                                uint8_t tag_length, const uint8_t *nonce,
                                                size_t nonce_length, const uint8_t *additional_data,
                                                size_t additional_data_length, const uint8_t *plaintext,
                                                size_t plaintext_length, uint8_t *ciphertext,
                                                size_t ciphertext_size, size_t *ciphertext_length)
{
    DEBUG("Peripheral ChaCha20-Poly1305 AEAD encryption\n");

    (void)attributes;
    (void)key_buffer_length;
    (void)nonce_length;
    (void)ciphertext_size;

    if (!cryptocell_310_data_within_ram(nonce) ||
        !cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(additional_data) ||
        !cryptocell_310_data_within_ram(plaintext)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    // todo: create chacha20poly1305 specific tag macro
    uint32_t tag[PSA_AEAD_TAG_MAX_SIZE];

    cryptocell_310_enable();
    CRYSError_t ret = CRYS_CHACHA_POLY((uint8_t *)nonce,
                                       key_buffer,
                                       CRYS_CHACHA_Encrypt,
                                       (uint8_t *)additional_data,
                                       additional_data_length,
                                       (uint8_t *)plaintext,
                                       plaintext_length,
                                       (uint8_t *)ciphertext,
                                       tag);
    cryptocell_310_disable();

    if (ret != CRYS_OK) {
        DEBUG("%s : cryptocell_310 failed to encrypt with %s.\n", __FILE__,
              cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    memcpy(&ciphertext[plaintext_length], tag, tag_length);
    *ciphertext_length = plaintext_length + tag_length;

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_decrypt(const psa_key_attributes_t *attributes,
                                                uint8_t *key_buffer, size_t key_buffer_length,
                                                uint8_t tag_length, const uint8_t *nonce,
                                                size_t nonce_length, const uint8_t *additional_data,
                                                size_t additional_data_length, const uint8_t *ciphertext,
                                                size_t ciphertext_length, uint8_t *plaintext,
                                                size_t plaintext_size, size_t *plaintext_length)
{
    DEBUG("Peripheral ChaCha20-Poly1305 AEAD decryption\n");

    (void)attributes;
    (void)key_buffer_length;
    (void)nonce_length;

    if (!cryptocell_310_data_within_ram(nonce) ||
        !cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(additional_data) ||
        !cryptocell_310_data_within_ram(ciphertext)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    /*
    if (ciphertext_length < tag_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t actual_plaintext_length = ciphertext_length - tag_length;
    if (plaintext_size < actual_plaintext_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }
    */

    uint32_t tag[PSA_AEAD_TAG_MAX_SIZE];
    memcpy(tag, &ciphertext[plaintext_size], tag_length);

    cryptocell_310_enable();
    CRYSError_t ret = CRYS_CHACHA_POLY((uint8_t *)nonce,
                                       key_buffer,
                                       CRYS_CHACHA_Decrypt,
                                       (uint8_t *)additional_data,
                                       additional_data_length,
                                       (uint8_t *)ciphertext,
                                       ciphertext_length - tag_length,
                                       (uint8_t *)plaintext,
                                       tag);
    cryptocell_310_disable();

    if (ret != CRYS_OK) {
        DEBUG("%s : cryptocell_310 failed to decrypt with %s.\n", __FILE__,
              cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    *plaintext_length = ciphertext_length - tag_length;

    return PSA_SUCCESS;
}

#ifdef __cplusplus
}
#endif
