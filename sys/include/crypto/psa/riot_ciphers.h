/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Contexts for the RIOT cipher module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef CRYPTO_PSA_RIOT_CIPHERS_H
#define CRYPTO_PSA_RIOT_CIPHERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto/ciphers.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_PSA_RIOT_CIPHER_AES_128_CBC)
typedef cipher_t psa_cipher_aes_128_ctx_t;
#endif
#if IS_USED(MODULE_PSA_RIOT_CIPHER_AES_192_CBC)
typedef cipher_t psa_cipher_aes_192_ctx_t;
#endif
#if IS_USED(MODULE_PSA_RIOT_CIPHER_AES_256_CBC)
typedef cipher_t psa_cipher_aes_256_ctx_t;
#endif
#if IS_USED(MODULE_PSA_RIOT_CIPHER_CHACHA20)
#include "crypto/chacha20poly1305.h"

/**
 * @brief This struct is used to handle the discontinuity between the PSA API
 * and the RIOT ChaCha20 API. The PSA API psa_cipher_update() function allows any size
 * of input array, but the RIOT implementation only works on multiples of 64B.
 * So we store any remainders from the input in the buffer, and if any more data
 * is input, we join them with the buffer values. The psa_cipher_finish() function
 * will then xcrypt the remaining buffer values.
 * The buffer is also used in the setup of the operation. The psa_cipher_~_setup()
 * functions only provide the key for the operation, but the ChaCha20 implementation
 * also needs the counter and nonce provided by psa_cipher_set_iv(). So we store
 * the key in the buffer until it is needed in the RIOT API setup.
 */
typedef struct {
    uint8_t buffer[CHACHA20POLY1305_BLOCK_BYTES];   /**< Block buffer */
    uint8_t buffer_length;                          /**< Current length of the data in buffer */
    chacha20_ctx_t ctx;                             /**< Context of the RIOT ChaCha20 operation */
} psa_cipher_chacha20_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_PSA_RIOT_CIPHERS_H */
/** @} */
