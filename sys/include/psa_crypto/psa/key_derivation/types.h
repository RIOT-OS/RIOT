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
 * @brief       Key derivation type definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Encoding of the step of a key derivation.
 */
typedef uint16_t psa_key_derivation_step_t;

/**
 * @brief   Structure storing a key derivation context
 *
 * @note    Not yet implemented
 */
struct psa_key_derivation_operation_s {
    int dummy;  /**< Not implemented yet */
};

/**
 * @brief   The type of the state object for key derivation operations.
 *
 * @details Before calling any function on a key derivation operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the object to all-bits-zero, for example:
 *            @code
 *            @ref psa_key_derivation_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the object to logical zero values by declaring the object as static or
 *            global without an explicit initializer, for example:
 *            @code
 *            static @ref psa_key_derivation_operation_t operation;
 *            @endcode
 *          - Initialize the object to the initializer @ref PSA_KEY_DERIVATION_OPERATION_INIT,
 *            for example:
 *            @code
 *            @ref psa_key_derivation_operation_t operation =
 *            @ref PSA_KEY_DERIVATION_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_key_derivation_operation_init() to
 *            the object, for example:
 *            @code
 *            @ref psa_key_derivation_operation_t operation;
 *            operation = @ref psa_key_derivation_operation_init();
 *            @endcode
 *          This is an implementation-defined type. Applications that make assumptions about the
 *          content of this object will result in in implementation-specific behavior, and are
 *          non-portable.
 */
typedef struct psa_key_derivation_operation_s psa_key_derivation_operation_t;

/**
 * @brief   This macro returns a suitable initializer for a key derivation operation object of
 *          type @ref psa_key_derivation_operation_t.
 */
#define PSA_KEY_DERIVATION_OPERATION_INIT { 0 }

/**
 * @brief   Return an initial value for a key derivation operation object.
 *
 * @return  psa_key_derivation_operation_t
 */
static inline psa_key_derivation_operation_t psa_key_derivation_operation_init(void)
{
    const psa_key_derivation_operation_t v = PSA_KEY_DERIVATION_OPERATION_INIT;

    return v;
}

#ifdef __cplusplus
}
#endif

/** @} */
