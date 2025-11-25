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

psa_status_t psa_generate_ecc_ed25519_key_pair( uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t *priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    *priv_key_buffer_length = EDSIGN_SECRET_KEY_SIZE;
    *pub_key_buffer_length = EDSIGN_PUBLIC_KEY_SIZE;

    random_bytes(priv_key_buffer, EDSIGN_SECRET_KEY_SIZE);
    ed25519_prepare(priv_key_buffer);
    edsign_sec_to_pub(pub_key_buffer, priv_key_buffer);

    return PSA_SUCCESS;
}

psa_status_t psa_derive_ecc_ed25519_public_key( const uint8_t *priv_key_buffer, uint8_t *pub_key_buffer,
                                                size_t priv_key_buffer_length,
                                                size_t *pub_key_buffer_length)
{
    *pub_key_buffer_length = EDSIGN_PUBLIC_KEY_SIZE;

    edsign_sec_to_pub(pub_key_buffer, priv_key_buffer);

    (void)priv_key_buffer_length;
    return PSA_SUCCESS;
}

psa_status_t psa_ecc_ed25519_sign_message(  const uint8_t *priv_key_buffer,
                                            size_t priv_key_buffer_size,
                                            const uint8_t *pub_key_buffer,
                                            size_t pub_key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *signature,
                                            size_t signature_size,
                                            size_t *signature_length)
{
    if (signature_size < EDSIGN_SIGNATURE_SIZE) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    *signature_length = EDSIGN_SIGNATURE_SIZE;
    edsign_sign(signature, pub_key_buffer, priv_key_buffer, input, input_length);

    (void)priv_key_buffer_size;
    (void)pub_key_buffer_size;
    return PSA_SUCCESS;
}

psa_status_t psa_ecc_ed25519_verify_message(const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            const uint8_t *signature,
                                            size_t signature_length)
{
    uint8_t ret = 0;

    if (signature_length < EDSIGN_SIGNATURE_SIZE) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    ret = edsign_verify(signature, key_buffer, input, input_length);
    if (!ret) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    (void)key_buffer_size;
    return PSA_SUCCESS;
}
