/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_psa_crypto pkg_monocypher
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the Monocypher EdDSA APIs
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include "string_utils.h"

#include "psa/crypto.h"
#include "psa_ecc.h"
#include "monocypher-ed25519.h"
#include "random.h"

psa_status_t psa_generate_ecc_ed25519_key_pair( uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer)
{
    /* todo: maybe this should use psa_random instead */
    random_bytes(priv_key_buffer, 32);

    return psa_derive_ecc_ed25519_public_key(priv_key_buffer, pub_key_buffer);
}

psa_status_t psa_derive_ecc_ed25519_public_key( const uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer)
{
    uint8_t priv_and_pub_key[64] = { 0 };

    memcpy(&priv_and_pub_key[0], priv_key_buffer, 32);
    crypto_ed25519_key_pair(priv_and_pub_key, pub_key_buffer, priv_and_pub_key);

    explicit_bzero(priv_and_pub_key, 64);

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_ed25519_sign_message(const uint8_t *priv_key_buffer,
                                        const uint8_t *pub_key_buffer,
                                        const uint8_t *input, size_t input_length,
                                        uint8_t *signature)
{
    uint8_t priv_and_pub_key[64];
    memcpy(&priv_and_pub_key[0], priv_key_buffer, 32);
    memcpy(&priv_and_pub_key[32], pub_key_buffer, 32);

    crypto_ed25519_sign(signature, priv_and_pub_key, input, input_length);

    explicit_bzero(priv_and_pub_key, 64);

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_ed25519_verify_message(const uint8_t *pub_key_buffer,
                                            const uint8_t *input, size_t input_length,
                                            const uint8_t *signature)
{
    if (crypto_ed25519_check(signature, pub_key_buffer, input, input_length) != 0) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}
