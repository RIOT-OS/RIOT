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
 * @brief       AEAD type definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/algorithm.h"
#include "psa/cipher/types.h"
/**
 * @brief   Structure storing an AEAD operation context
 *
 * @note    Not implemented, yet
 */
struct psa_aead_operation_s {
    uint8_t nonce_set : 1;        /**< True if Nonce was already set */
    uint8_t nonce_required : 1;   /**< True if IV was already set */
    uint8_t default_nonce_length; /**< Default IV length for algorithm */
    psa_algorithm_t alg;          /**< Operation algorithm*/
    /** Union containing AEAD cipher contexts for the executing backend */
    union aead_cipher_context {
        psa_cipher_context_t cipher_ctx; /**< Cipher context */
    } backend_ctx;

    int dummy; /**< Not implemented, yet */
};

/* These are all temporarily defined as some numeric type to prevent errors at compile time.*/
/**
 * @brief   The type of the state object for multi-part AEAD operations.
 *
 * @details Before calling any function on an AEAD operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_aead_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static
 *            or global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_aead_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_AEAD_OPERATION_INIT, for example:
 *            @code
 *            @ref psa_aead_operation_t operation = @ref PSA_AEAD_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_aead_operation_init() to the object,
 *            for example:
 *            @code
 *            @ref psa_aead_operation_t operation;
 *            operation = @ref psa_aead_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_aead_operation_s psa_aead_operation_t;

/**
 * @brief   This macro returns a suitable initializer for an AEAD operation object of type
 *          @ref psa_aead_operation_t.
 */
#define PSA_AEAD_OPERATION_INIT \
    {                           \
        0                       \
    }

/**
 * @brief   Return an initial value for an AEAD operation object.
 *
 * @return  psa_aead_operation_t
 */
static inline psa_aead_operation_t psa_aead_operation_init(void)
{
    const psa_aead_operation_t v = PSA_AEAD_OPERATION_INIT;

    return v;
}

#ifdef __cplusplus
}
#endif

/** @} */
