/*
 * Copyright (C) 2025 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       Cipher type definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto/psa/riot_ciphers.h"
#include "kernel_defines.h"
#include "psa/algorithm.h"

#if IS_USED(MODULE_PERIPH_CIPHER_AES_128_CBC)
#include "psa_periph_aes_ctx.h"
#endif

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
#include "atca_params.h"
#endif

/**
 * @brief   For encrypt-decrypt functions, whether the operation is an encryption
 *          or a decryption.
 */
typedef enum {
    PSA_CRYPTO_DRIVER_DECRYPT,
    PSA_CRYPTO_DRIVER_ENCRYPT
} psa_encrypt_or_decrypt_t;

/**
 * @brief   Structure containing the cipher contexts needed by the application.
 */
typedef union {
#if IS_USED(MODULE_PSA_CIPHER_AES_128_ECB) ||\
    IS_USED(MODULE_PSA_CIPHER_AES_128_CBC) ||\
    defined(DOXYGEN)
    psa_cipher_aes_128_ctx_t aes_128;   /**< AES 128 context*/
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_192_CBC) || defined(DOXYGEN)
    psa_cipher_aes_192_ctx_t aes_192;   /**< AES 192 context*/
#endif
#if IS_USED(MODULE_PSA_CIPHER_AES_256_CBC) || defined(DOXYGEN)
    psa_cipher_aes_256_ctx_t aes_256;   /**< AES 256 context*/
#endif
} psa_cipher_context_t;

/**
 * @brief   Structure containing the secure element specific cipher contexts needed by the
 *          application.
 */
typedef struct {
    psa_encrypt_or_decrypt_t direction; /**< Direction of this cipher operation */
    /** Structure containing a driver specific cipher context */
    union driver_context {
        unsigned dummy; /**< Make the union non-empty even with no supported algorithms. */
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A) || defined(DOXYGEN)
        atca_aes_cbc_ctx_t atca_aes_cbc;    /**< ATCA AES CBC context*/
    #endif
    } drv_ctx;  /**< SE specific cipher operation context */
} psa_se_cipher_context_t;

/**
 * @brief   Structure storing a cipher operation context
 */
struct psa_cipher_operation_s {
    uint8_t iv_required : 1;        /**< True if algorithm requires IV */
    uint8_t iv_set : 1;             /**< True if IV was already set */
    uint8_t default_iv_length;      /**< Default IV length for algorithm */
    psa_algorithm_t alg;            /**< Operation algorithm*/
    /** Union containing cipher contexts for the executing backend */
    union cipher_context {
        psa_cipher_context_t cipher_ctx;    /**< Cipher context */
#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A) || defined(DOXYGEN)
        psa_se_cipher_context_t se_ctx;     /**< SE Cipher context */
#endif
    } backend_ctx;  /**< Backend specific cipher context */
};

/**
 * @brief   The type of the state object for multi-part cipher operations.
 *
 * @details Before calling any function on a cipher operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_cipher_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static or
 *            global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_cipher_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_CIPHER_OPERATION_INIT, for example:
 *            @code
 *            @ref psa_cipher_operation_t operation = @ref PSA_CIPHER_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_cipher_operation_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_cipher_operation_t operation;
 *            operation = @ref psa_cipher_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_cipher_operation_s psa_cipher_operation_t;

/**
 * @brief   This macro returns a suitable initializer for a cipher operation
 *          object of type @ref psa_cipher_operation_t.
 */
#define PSA_CIPHER_OPERATION_INIT { 0 }

/**
 * @brief   Return an initial value for a cipher operation object.
 *
 * @return  psa_cipher_operation_t
 */
static inline psa_cipher_operation_t psa_cipher_operation_init(void)
{
    const psa_cipher_operation_t v = PSA_CIPHER_OPERATION_INIT;

    return v;
}

#ifdef __cplusplus
}
#endif

/** @} */
