/*
 * Copyright (C) 2024 TU Dresden
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
 * @brief       CryptoCell 310 driver specific ChaCha20 contexts
 *
 * @author      Lennard Melling <lennard.melling@msx.tu-dresden.de>
 *
 */
#ifndef PSA_PERIPH_CHACHA20_CTX_H
#define PSA_PERIPH_CHACHA20_CTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "crys_chacha.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_PERIPH_CIPHER_CHACHA20) || DOXYGEN
/**
 * @brief This struct is used to handle the discontinuity between the PSA API
 * and the CRYS ChaCha20 API. The PSA API psa_cipher_update() function allows any size
 * of input array, but the CRYS implementation only works on multiples of 64B.
 * So we store any remainders from the input in the buffer, and if any more data
 * is input, we join them with the buffer values. The psa_cipher_finish() function
 * will then xcrypt the remaining buffer values.
 * The buffer is also used in the setup of the operation. The psa_cipher_~_setup()
 * functions only provide the key for the operation, but the CRYS ChaCha20
 * implementation also needs the counter and nonce provided by psa_cipher_set_iv().
 * So we store the key in the buffer until it is needed in the CRYS ChaCha20 setup.
 */
typedef struct {
    uint8_t buffer[CRYS_CHACHA_BLOCK_SIZE_IN_BYTES];    /**< Block buffer */
    uint8_t buffer_length;                              /**< Current length of the data in buffer */
    union {
        CRYS_CHACHA_EncryptMode_t mode;                 /**< Mode of the operation */
        CRYS_CHACHAUserContext_t post_setup;            /**< Context of the CRYS ChaCha20 operation */
    } ctx;                                              /**< Context Setup values */
} psa_cipher_chacha20_ctx_t;

#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_PERIPH_CHACHA20_CTX_H */
/** @} */
