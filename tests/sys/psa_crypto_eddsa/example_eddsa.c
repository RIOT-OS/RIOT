/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA EDDSA configurations
 *              Contents have been copied from `examples/advanced_examples/psa_crypto`
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "psa/crypto.h"

#define EDDSA_MESSAGE_SIZE  (127)

#define ECC_KEY_SIZE (255)
#define ECC_KEY_TYPE (PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS))
#define ECC_ALG (PSA_ALG_PURE_EDDSA)

/**
 * @brief   Example function to perform an EdDSA operation with the twisted Edwards curve Edwards25519
 *          with the PSA Crypto API.
 */
psa_status_t example_eddsa(void)
{
    psa_key_id_t privkey_id;
    psa_key_attributes_t privkey_attr = psa_key_attributes_init();
    psa_key_id_t pubkey_id;
    psa_key_attributes_t pubkey_attr = psa_key_attributes_init();

    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_VERIFY_MESSAGE;

    uint8_t public_key[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE)] = { 0 };
    size_t pubkey_length;
    uint8_t signature[PSA_SIGN_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE, ECC_ALG)];
    size_t sig_length;
    uint8_t msg[EDDSA_MESSAGE_SIZE] = { 0x0b };

    psa_set_key_algorithm(&privkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&privkey_attr, usage);
    psa_set_key_type(&privkey_attr, ECC_KEY_TYPE);
    psa_set_key_bits(&privkey_attr, ECC_KEY_SIZE);

    psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;

    status = psa_generate_key(&privkey_attr, &privkey_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_export_public_key(privkey_id, public_key, sizeof(public_key), &pubkey_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    psa_set_key_algorithm(&pubkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&pubkey_attr, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_bits(&pubkey_attr, PSA_BYTES_TO_BITS(pubkey_length));
    psa_set_key_type(&pubkey_attr, PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(ECC_KEY_TYPE));

    status = psa_import_key(&pubkey_attr, public_key, pubkey_length, &pubkey_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = psa_sign_message(privkey_id, ECC_ALG, msg, sizeof(msg), signature, sizeof(signature),
                           &sig_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_verify_message(pubkey_id, ECC_ALG, msg, sizeof(msg), signature, sig_length);
}
