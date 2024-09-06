#include "psa/crypto.h"
#include "psa_ecc.h"

#include "tee_ecc.h"

psa_status_t psa_generate_ecc_p256r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    *priv_key_buffer_length = PSA_BITS_TO_BYTES(attributes->bits);
    *pub_key_buffer_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(attributes->type, attributes->bits);

    /**
     * Add 0x04 prefix for SEC 1 encoded uncompressed elliptic curve points.
     * Micro-ECC represents public keys in SEC 1 uncompressed format without the 0x04 prefix [1].
     * PSA Crypto uses a standard SEC 1 uncompressed representation [2].
     * [1] https://github.com/kmackay/micro-ecc/blob/master/README.md#point-representation
     * [2] https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html#key-formats
     */
    pub_key_buffer[0] = 0x04;
    psa_status_t status = tee_generate_ecc_p256r1_key_pair(priv_key_buffer, pub_key_buffer+1, *priv_key_buffer_length, (*pub_key_buffer_length) - 1);

    if (status != PSA_SUCCESS) {
        return status;
    }
    return PSA_SUCCESS;
}

psa_status_t psa_import_ecc_p256r1_key_pair(const psa_key_attributes_t *attributes, const uint8_t *key_data, size_t key_data_length, uint8_t *priv_key_buffer, size_t priv_key_size, size_t *priv_key_buffer_length, uint8_t *pub_key_buffer, size_t *pub_key_buffer_length)
{
    *priv_key_buffer_length = PSA_BITS_TO_BYTES(attributes->bits);
    *pub_key_buffer_length = PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(attributes->type, attributes->bits);

    /**
     * Add 0x04 prefix for SEC 1 encoded uncompressed elliptic curve points.
     * Micro-ECC represents public keys in SEC 1 uncompressed format without the 0x04 prefix [1].
     * PSA Crypto uses a standard SEC 1 uncompressed representation [2].
     * [1] https://github.com/kmackay/micro-ecc/blob/master/README.md#point-representation
     * [2] https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html#key-formats
     */
    pub_key_buffer[0] = 0x04;
    psa_status_t status = tee_import_ecc_p256r1_key_pair(key_data, key_data_length, priv_key_buffer, pub_key_buffer+1, priv_key_size, (*pub_key_buffer_length) - 1);

    if (status != PSA_SUCCESS) {
        return status;
    }

    *priv_key_buffer_length = priv_key_size;
    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p256r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *hash, size_t hash_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length)
{
    if (alg != PSA_ALG_ECDSA(PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    (void) attributes;
    return tee_ecc_p256r1_sign_hash(key_buffer, key_buffer_size, hash, hash_length, signature, signature_size, signature_length);
}

psa_status_t psa_ecc_p256r1_sign_message(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        uint8_t *signature, size_t signature_size,
                                        size_t *signature_length)
{
    if (alg != PSA_ALG_ECDSA(PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    (void) attributes;
    return tee_ecc_p256r1_sign_message(key_buffer, key_buffer_size, input, input_length, signature, signature_size, signature_length);
}

psa_status_t psa_ecc_p256r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *hash, size_t hash_length,
                                        const uint8_t *signature, size_t signature_length)
{
    if (alg != PSA_ALG_ECDSA(PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    (void) attributes;
    return tee_ecc_p256r1_verify_hash(key_buffer, key_buffer_size, hash, hash_length, signature, signature_length);
}

psa_status_t psa_ecc_p256r1_verify_message(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg,
                                        const uint8_t *key_buffer, size_t key_buffer_size,
                                        const uint8_t *input, size_t input_length,
                                        const uint8_t *signature, size_t signature_length)
{
    if (alg != PSA_ALG_ECDSA(PSA_ALG_SHA_256)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    (void) attributes;
    return tee_ecc_p256r1_verify_message(&key_buffer[1], key_buffer_size-1, input, input_length, signature, signature_length);
}
