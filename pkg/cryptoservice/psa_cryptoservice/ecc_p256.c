#include "psa/crypto.h"

#include "CYS/protected_key.h"
#include "CYS/unprotected.h"

psa_status_t psa_generate_ecc_p256r1_key_pair(  const psa_key_attributes_t *attributes,
                                                uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    psa_status_t status;
    psa_location_t location = PSA_LIFETIME_GET_LOCATION(attributes->lifetime);

    switch(location) {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            status = CYS_p256_generate(priv_key_buffer, pub_key_buffer);
            break;
        case PSA_KEY_LOCATION_LOCAL_SEALED:
            status = CYS_PROT_p256_generate(priv_key_buffer, pub_key_buffer);
            break;
        default:
            return PSA_ERROR_NOT_SUPPORTED;
    }

    if (status != PSA_SUCCESS) {
        return status;
    }

    *priv_key_buffer_length = CYS_P256_KEY_SIZE;
    *pub_key_buffer_length = CYS_P256_PUB_SIZE;

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p256r1_sign_hash(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *hash,
                                        size_t hash_length, uint8_t *signature,
                                        size_t signature_size, size_t *signature_length)
{
    (void)alg;
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;
    psa_location_t location = PSA_LIFETIME_GET_LOCATION(attributes->lifetime);

    if(signature_size < CYS_P256_SIG_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if (location == PSA_KEY_LOCATION_LOCAL_STORAGE) {

        if (key_buffer_size < CYS_P256_KEY_SIZE) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = CYS_p256_sign(key_buffer, hash, hash_length, signature);
    }
    else if (location == PSA_KEY_LOCATION_LOCAL_SEALED) {
        if (key_buffer_size < sizeof(CYS_PROT_p256_key_t)) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }
        status = CYS_PROT_p256_sign(key_buffer, hash, hash_length, signature);
    }

    if (status != PSA_SUCCESS) {
        return status;
    }

    *signature_length = CYS_P256_SIG_SIZE;

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p256r1_sign_message(  const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *input,
                                        size_t input_length, uint8_t *signature,
                                        size_t signature_size, size_t *signature_length)
{
    uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))];
    size_t hash_length;

    psa_status_t status = psa_hash_compute(PSA_ALG_GET_HASH(alg), input, input_length, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_ecc_p256r1_sign_hash(attributes, alg, key_buffer, key_buffer_size, hash, hash_length, signature, signature_size, signature_length);
}

psa_status_t psa_ecc_p256r1_verify_hash(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *hash,
                                        size_t hash_length, const uint8_t *signature,
                                        size_t signature_length)
{
    (void)alg;
    (void)attributes;

    if(key_buffer_size < CYS_P256_PUB_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if(signature_length != CYS_P256_SIG_SIZE) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_status_t status;
    status = CYS_p256_verify(key_buffer, hash, hash_length, signature);
    if(status != PSA_SUCCESS) {
        return status;
    }

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_p256r1_verify_message(const psa_key_attributes_t *attributes,
                                        psa_algorithm_t alg, const uint8_t *key_buffer,
                                        size_t key_buffer_size, const uint8_t *input,
                                        size_t input_length, const uint8_t *signature,
                                        size_t signature_length)
{
    uint8_t hash[PSA_HASH_LENGTH(PSA_ALG_GET_HASH(alg))];
    size_t hash_length;

    psa_status_t status = psa_hash_compute(PSA_ALG_GET_HASH(alg), input, input_length, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_ecc_p256r1_verify_hash(attributes, alg, key_buffer, key_buffer_size, hash, hash_length, signature, signature_length);
}
