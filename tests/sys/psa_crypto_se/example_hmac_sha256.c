/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA secure element configurations
 *              Contents have been copied from `examples/psa_crypto`
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "psa/crypto.h"

static const uint8_t HMAC_KEY[] = {
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
    0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};
static size_t HMAC_KEY_LEN = 32;

static const uint8_t HMAC_MSG[] = {
    0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
    0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0x73, 0x74,
    0x72, 0x69, 0x6e, 0x67, 0x20, 0x66, 0x6f, 0x72,
    0x20, 0x68, 0x6d, 0x61, 0x63, 0x32, 0x35, 0x36
};
static size_t HMAC_MSG_LEN = 32;

/**
 * @brief   Example function to perform an HMAC SHA-256 computation
 *          with the PSA Crypto API.
 *
 * @return  psa_status_t
 */
psa_status_t example_hmac_sha256(void)
{
    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_key_id_t key_id = 0;
    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_MESSAGE;

    size_t digest_size =
        PSA_MAC_LENGTH(PSA_KEY_TYPE_HMAC, HMAC_KEY_LEN, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    uint8_t digest[digest_size];
    size_t output_len = 0;

    psa_set_key_algorithm(&attr, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_usage_flags(&attr, usage);
    psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(HMAC_KEY_LEN));
    psa_set_key_type(&attr, PSA_KEY_TYPE_HMAC);

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
        PSA_KEY_LIFETIME_VOLATILE, PSA_ATCA_LOCATION_DEV0);
    psa_set_key_lifetime(&attr, lifetime);

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
    status = psa_import_key(&attr, HMAC_KEY, HMAC_KEY_LEN, &key_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_mac_compute(key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256),
                           HMAC_MSG, HMAC_MSG_LEN, digest, digest_size,
                           &output_len);
}
