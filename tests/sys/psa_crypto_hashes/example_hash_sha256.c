/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA hash configurations
 *              Contents have been copied from `examples/psa_crypto`
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"

static const uint8_t HASH_MSG[] = {
    0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
    0x61, 0x20, 0x74, 0x65, 0x73, 0x74, 0x73, 0x74,
    0x72, 0x69, 0x6e, 0x67, 0x20, 0x66, 0x6f, 0x72,
    0x20, 0x68, 0x6d, 0x61, 0x63, 0x32, 0x35, 0x36
};

psa_status_t example_hash_sha256(void)
{
    uint8_t hash_out[PSA_HASH_LENGTH(PSA_ALG_SHA_256)];
    size_t hash_length;

    return psa_hash_compute(PSA_ALG_SHA_256, HASH_MSG, sizeof(HASH_MSG), hash_out, sizeof(hash_out), &hash_length);
}
