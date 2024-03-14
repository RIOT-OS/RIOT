/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 EC 25519 curve APIs
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include "crys_ec_edw_api.h"
#include "psa_error.h"
#include "cryptocell_310_util.h"
#include "string_utils.h"

#define ENABLE_DEBUG    0
#include "debug.h"

extern CRYS_RND_State_t *rndState_ptr;

psa_status_t psa_generate_ecc_ed25519_key_pair( uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    CRYS_ECEDW_TempBuff_t tmp;
    CRYSError_t ret;

    /* contains seed (private key), concatenated with public key */
    uint8_t secret_key[CRYS_ECEDW_ORD_SIZE_IN_BYTES + CRYS_ECEDW_MOD_SIZE_IN_BYTES] = { 0x0 };
    size_t secret_key_size = sizeof(secret_key);

    *priv_key_buffer_length = CRYS_ECEDW_ORD_SIZE_IN_BYTES;
    *pub_key_buffer_length = CRYS_ECEDW_MOD_SIZE_IN_BYTES;

    cryptocell_310_enable();
    ret = CRYS_ECEDW_KeyPair(secret_key, &secret_key_size,
                                        pub_key_buffer, pub_key_buffer_length,
                                        rndState_ptr, CRYS_RND_GenerateVector, &tmp);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECEDW_KeyPair failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        goto done;
    }

    memcpy(priv_key_buffer, secret_key, CRYS_ECEDW_ORD_SIZE_IN_BYTES);

done:
    explicit_bzero(&secret_key, sizeof(secret_key));
    return CRYS_to_psa_error(ret);
}

psa_status_t psa_derive_ecc_ed25519_public_key( const uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    CRYS_ECEDW_TempBuff_t tmp;
    CRYSError_t ret;

    /* contains seed (private key), concatenated with public key */
    uint8_t secret_key[CRYS_ECEDW_ORD_SIZE_IN_BYTES + CRYS_ECEDW_MOD_SIZE_IN_BYTES] = { 0x0 };
    size_t secret_key_size = sizeof(secret_key);

    *pub_key_buffer_length = CRYS_ECEDW_MOD_SIZE_IN_BYTES;

    cryptocell_310_enable();
    ret = CRYS_ECEDW_SeedKeyPair(priv_key_buffer, priv_key_buffer_length, secret_key, &secret_key_size,
                                            pub_key_buffer, pub_key_buffer_length, &tmp);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECEDW_SeedKeyPair failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        goto done;
    }

done:
    explicit_bzero(&secret_key, sizeof(secret_key));
    return CRYS_to_psa_error(ret);
}

psa_status_t psa_ecc_ed25519_sign_message(const uint8_t *priv_key_buffer,
                                        size_t priv_key_buffer_size,
                                        const uint8_t *pub_key_buffer,
                                        size_t pub_key_buffer_size,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *signature,
                                        size_t signature_size,
                                        size_t *signature_length)
{
    CRYS_ECEDW_TempBuff_t tmp;
    CRYSError_t ret;

    if (input_length > (CRYS_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES - CRYS_ECEDW_SIGNATURE_BYTES)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* contains seed (private key), concatenated with public key */
    uint8_t secret_key[CRYS_ECEDW_ORD_SIZE_IN_BYTES + CRYS_ECEDW_MOD_SIZE_IN_BYTES] = { 0x0 };

    if (priv_key_buffer_size != CRYS_ECEDW_ORD_SIZE_IN_BYTES || pub_key_buffer_size != CRYS_ECEDW_MOD_SIZE_IN_BYTES) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    memcpy(secret_key, priv_key_buffer, CRYS_ECEDW_ORD_SIZE_IN_BYTES);
    memcpy(&secret_key[CRYS_ECEDW_ORD_SIZE_IN_BYTES], pub_key_buffer, CRYS_ECEDW_MOD_SIZE_IN_BYTES);
    *signature_length = signature_size;

    cryptocell_310_enable();
    ret = CRYS_ECEDW_Sign(signature, signature_length, input, input_length, secret_key, sizeof(secret_key), &tmp);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECEDW_Sign failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        goto done;
    }

done:
    explicit_bzero(&secret_key, sizeof(secret_key));
    return CRYS_to_psa_error(ret);

    (void)signature_size;
}

psa_status_t psa_ecc_ed25519_verify_message(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            const uint8_t *signature,
                                            size_t signature_length)
{
    CRYS_ECEDW_TempBuff_t tmp;
    CRYSError_t ret;

    if (input_length > (CRYS_HASH_UPDATE_DATA_MAX_SIZE_IN_BYTES - CRYS_ECEDW_SIGNATURE_BYTES)) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    cryptocell_310_enable();
    ret = CRYS_ECEDW_Verify(signature, signature_length, key_buffer, key_buffer_size, (uint8_t *)input, input_length, &tmp);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_ECEDW_Verify failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    return PSA_SUCCESS;
}
