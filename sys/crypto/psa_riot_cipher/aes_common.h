/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @cond
 * @ingroup     sys_crypto
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the RIOT Cipher module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "crypto/modes/cbc.h"

/**
 * @brief   Converts errors of the RIOT cipher module to PSA status values
 */
psa_status_t cipher_to_psa_error(int error);

/**
 * @brief   Common AES CBC Encrypt function
 *
 * @return  @ref psa_status_t
 */
psa_status_t cbc_aes_common_encrypt_decrypt(cipher_t *ctx,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    const uint8_t *iv,
                                    const uint8_t *input,
                                    size_t input_length,
                                    uint8_t *output,
                                    size_t *output_length,
                                    psa_encrypt_or_decrypt_t direction);

#ifdef __cplusplus
}
#endif

/** @} */
/** @endcond */
