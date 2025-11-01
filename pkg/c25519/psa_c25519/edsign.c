/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_c25519
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the C25519 APIs
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "ed25519.h"
#include "edsign.h"
#include "random.h"

psa_status_t psa_generate_ecc_ed25519_key_pair( uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer)
{
    /* todo: maybe this should usa psa_random instead */
    random_bytes(priv_key_buffer, EDSIGN_SECRET_KEY_SIZE);
    ed25519_prepare(priv_key_buffer);
    edsign_sec_to_pub(pub_key_buffer, priv_key_buffer);

    return PSA_SUCCESS;
}

psa_status_t psa_derive_ecc_ed25519_public_key( const uint8_t *priv_key_buffer,
                                                uint8_t *pub_key_buffer)
{
    edsign_sec_to_pub(pub_key_buffer, priv_key_buffer);

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_ed25519_sign_message(const uint8_t *priv_key_buffer,
                                        const uint8_t *pub_key_buffer,
                                        const uint8_t *input, size_t input_length,
                                        uint8_t *signature)
{
    edsign_sign(signature, pub_key_buffer, priv_key_buffer, input, input_length);

    return PSA_SUCCESS;
}

psa_status_t psa_ecc_ed25519_verify_message(const uint8_t *pub_key_buffer,
                                            const uint8_t *input, size_t input_length,
                                            const uint8_t *signature)
{
    if (!edsign_verify(signature, pub_key_buffer, input, input_length)) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}
