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
 * @file
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_PERIPH_AES_COMMON_H
#define PSA_PERIPH_AES_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

/**
 * @brief   Common setup function for AES operations
 */
psa_status_t common_aes_setup(SaSiAesUserContext_t *ctx,
                              SaSiAesEncryptMode_t direction,
                              SaSiAesOperationMode_t mode,
                              SaSiAesPaddingType_t padding,
                              const uint8_t *iv,
                              const uint8_t *key_buffer,
                              size_t key_buffer_size);

/**
 * @brief   Common function for an AES encryption
 */
psa_status_t common_aes_encrypt(SaSiAesUserContext_t *ctx,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_buffer_size,
                                size_t *output_length);

#ifdef __cplusplus
}
#endif

#endif /* PSA_PERIPH_AES_COMMON_H */
/** @} */
