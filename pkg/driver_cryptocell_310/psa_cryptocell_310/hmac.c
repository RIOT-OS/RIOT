/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "kernel_defines.h"
#include "psa/crypto.h"
#include "psa_error.h"

#include "cryptocell_310_util.h"
#include "crys_hmac.h"
#include "crys_hmac_error.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_mac_compute_hmac_sha256(const psa_key_attributes_t *attributes,
                                         const uint8_t *key_buffer,
                                         size_t key_buffer_size,
                                         const uint8_t *input,
                                         size_t input_length,
                                         uint8_t *mac,
                                         size_t mac_size,
                                         size_t *mac_length)
{
    CRYSError_t ret;
    size_t required_mac_length =
        PSA_MAC_LENGTH(attributes->type, attributes->bits, PSA_ALG_SHA_256);

    if (!cryptocell_310_data_within_ram(key_buffer) ||
        !cryptocell_310_data_within_ram(input)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", __FILE__);
        return PSA_ERROR_DATA_INVALID;
    }

    if (mac_size < required_mac_length) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    ret = CRYS_HMAC(CRYS_HASH_SHA256_mode, (uint8_t *)key_buffer, key_buffer_size, (uint8_t *)input,
                    input_length, (uint32_t *)mac);
    if (ret != CRYS_OK) {
        DEBUG("CRYS_HMAC failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }

    *mac_length =  required_mac_length;

    (void)mac_size;
    return PSA_SUCCESS;
}
