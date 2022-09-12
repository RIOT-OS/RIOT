/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_driver_cryptocell
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

#include "crys_hmac.h"
#include "crys_hmac_error.h"

#define ENABLE_DEBUG    0
#include "debug.h"

psa_status_t psa_mac_compute_hmac_sha256(   const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *mac,
                                            size_t mac_size,
                                            size_t *mac_length)
{
    CRYSError_t ret;

    DEBUG("Periph HMAC SHA256\n");
    ret = CRYS_HMAC(CRYS_HASH_SHA256_mode, (uint8_t *)key_buffer, key_buffer_size, (uint8_t *)input,
                    input_length, (uint32_t *)mac);
    if (ret != CRYS_OK) {
        return CRYS_to_psa_error(ret);
    }

    *mac_length = 32;
    (void)attributes;
    (void)mac_size;
    return PSA_SUCCESS;
}
