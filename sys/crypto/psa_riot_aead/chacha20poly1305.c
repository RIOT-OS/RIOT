#include <stdio.h>
#include "psa/crypto.h"
#include "crypto/chacha20poly1305.h"

psa_status_t psa_aead_chacha20_poly1305_encrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *plaintext,
                                          size_t plaintext_length, uint8_t *ciphertext,
                                          size_t ciphertext_size, size_t *ciphertext_length) {
    (void)attributes;
    (void)key_buffer_length;
    (void)tag_length;
    (void)ciphertext_size;
    (void)ciphertext_length;

    /* Only 12 and 8 byte nonces are supported. */
    switch(nonce_length) {
        case 12:
        case 8:
            break;
        default:
            return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20poly1305_encrypt(ciphertext, plaintext, plaintext_length,
                             additional_data, additional_data_length,
                             key_buffer, nonce);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_decrypt(const psa_key_attributes_t *attributes,
                                          uint8_t *key_buffer, size_t key_buffer_length,
                                          uint8_t tag_length, const uint8_t *nonce,
                                          size_t nonce_length, const uint8_t *additional_data,
                                          size_t additional_data_length, const uint8_t *ciphertext,
                                          size_t ciphertext_length, uint8_t *plaintext,
                                          size_t plaintext_size, size_t *plaintext_length) {
    (void)attributes;
    (void)key_buffer_length;
    (void)tag_length;
    (void)plaintext_size;
    
    /* Only 12 and 8 byte nonces are supported. */
    switch(nonce_length) {
        case 12:
        case 8:
            break;
        default:
            return PSA_ERROR_INVALID_ARGUMENT;
    }

    chacha20poly1305_decrypt(ciphertext, ciphertext_length,
                             plaintext, plaintext_length,
                             additional_data, additional_data_length,
                             key_buffer, nonce);

    return PSA_SUCCESS;
}

psa_status_t psa_aead_chacha20_poly1305_encrypt_setup(psa_aead_operation_t *operation,
                                          const psa_key_attributes_t *attributes,
                                          const psa_key_slot_t *slot,
                                          psa_algorithm_t alg)
 {    (void)operation;
    (void)attributes;
    (void)slot;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_chacha20_poly1305_decrypt_setup(psa_aead_operation_t *operation,
                                          const psa_key_attributes_t *attributes,
                                          const psa_key_slot_t *slot,
                                          psa_algorithm_t alg)
 {
    (void)operation;
    (void)attributes;
    (void)slot;
    (void)alg;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_chacha20_poly1305_set_lengths(psa_aead_operation_t *operation,
                                          size_t ad_length,
                                          size_t plaintext_length)
 {
    (void)operation;
    (void)ad_length;
    (void)plaintext_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t psa_aead_chacha20_poly1305_generate_nonce(psa_aead_operation_t *operation,
                                          uint8_t *nonce,
                                          size_t nonce_size,
                                          size_t *nonce_length)
 {
    (void)operation;
    (void)nonce;
    (void)nonce_size;
    (void)nonce_length;
    return PSA_ERROR_NOT_SUPPORTED;
}
