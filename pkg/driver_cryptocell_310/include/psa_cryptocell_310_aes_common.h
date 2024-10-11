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
 * @file
 * @brief       Common AES functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 AES APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTOCELL_310_AES_COMMON_H
#define PSA_CRYPTOCELL_310_AES_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

/**
 * @brief   Common setup function for AES operations
 *
 * @param   ctx                 Driver specific AES context of the type @c SaSiAesUserContext_t
 * @param   direction           Encrypt or decrypt direction of type @c SaSiAesEncryptMode_t
 * @param   mode                Operation mode (e.g. CBC, CCM) of type @c SaSiAesOperationMode_t
 * @param   padding             Operation padding type of type @c SaSiAesPaddingType_t
 * @param   iv                  Constant buffer containing the IV for the operation
 * @param   key_buffer          Constant buffer containing an AES key
 * @param   key_size            Size of AES key in use.
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_aes_setup(SaSiAesUserContext_t *ctx,
                              SaSiAesEncryptMode_t direction,
                              SaSiAesOperationMode_t mode,
                              SaSiAesPaddingType_t padding,
                              const uint8_t *iv,
                              const uint8_t *key_buffer,
                              size_t key_size);

/**
 * @brief   Common function for an AES encryption
 *
 * @param   ctx                 AES context of the type @c SaSiAesUserContext_t
 * @param   input               Constant input buffer of plain text to encrypt
 * @param   input_length        Length of the input buffer
 * @param   output              Output buffer to write the cipher
 * @param   output_buffer_size  Size of the output buffer. Must be at least @c input_length
 * @param   output_length       Pointer to output length. Will contain actual length of cipher
 * @return  psa_status_t
 */
psa_status_t cryptocell_310_common_aes_encrypt_decrypt(SaSiAesUserContext_t *ctx,
                                        const uint8_t *input,
                                        size_t input_length,
                                        uint8_t *output,
                                        size_t output_buffer_size,
                                        size_t *output_length);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PSA_CRYPTOCELL_310_AES_COMMON_H */
