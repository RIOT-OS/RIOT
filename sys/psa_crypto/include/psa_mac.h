/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_mac  PSA Wrapper Functions: MAC
 * @{
 *
 * @file        psa_mac.h
 * @brief       Function declarations for low level wrapper functions for MAC operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_MAC_H
#define PSA_MAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/crypto.h"
#include "psa/crypto_contexts.h"

/**
 * @brief   Low level wrapper function to call a driver for a HMAC SHA256 computation
 *          See psa_mac_compute()
 *
 * @param   attributes
 * @param   key_buffer
 * @param   key_buffer_size
 * @param   input
 * @param   input_length
 * @param   mac
 * @param   mac_size
 * @param   mac_length
 * @return  psa_status_t
 */
psa_status_t psa_mac_compute_hmac_sha256(   const psa_key_attributes_t *attributes,
                                            const uint8_t *key_buffer,
                                            size_t key_buffer_size,
                                            const uint8_t *input,
                                            size_t input_length,
                                            uint8_t *mac,
                                            size_t mac_size,
                                            size_t *mac_length);

#ifdef __cplusplus
}
#endif

#endif /* PSA_MAC_H */
/**@}*/
