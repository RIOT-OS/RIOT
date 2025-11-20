/*
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
 * @file        crypto_struct.h
 * @brief       Structure definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto_types.h"
#include "crypto_sizes.h"
#include "crypto_contexts.h"

#if IS_USED(MODULE_PSA_AEAD) || defined(DOXYGEN)
/**
 * @brief   Structure storing an AEAD operation context
 *
 * @note    Not implemented, yet
 */
struct psa_aead_operation_s {
    int dummy;  /**< Not implemented, yet */
};

/**
 * @brief   This macro returns a suitable initializer for an AEAD operation object of type
 *          @ref psa_aead_operation_t.
 */
#define PSA_AEAD_OPERATION_INIT { 0 }

/**
 * @brief   Return an initial value for an AEAD operation object.
 *
 * @return  psa_aead_operation_s
 */
static inline struct psa_aead_operation_s psa_aead_operation_init(void)
{
    const struct psa_aead_operation_s v = PSA_AEAD_OPERATION_INIT;

    return v;
}
#endif /* MODULE_PSA_AEAD */

#if IS_USED(MODULE_PSA_KEY_DERIVATION) || defined(DOXYGEN)
/**
 * @brief   This macro returns a suitable initializer for a key derivation operation object of
 *          type @ref psa_key_derivation_operation_t.
 */
#define PSA_KEY_DERIVATION_OPERATION_INIT { 0 }

/**
 * @brief   Structure storing a key derivation context
 *
 * @note    Not yet implemented
 */
struct psa_key_derivation_operation_s {
    int dummy;  /**< Not implemented yet */
};

/**
 * @brief   Return an initial value for a key derivation operation object.
 *
 * @return  psa_key_derivation_operation_s
 */
static inline struct psa_key_derivation_operation_s psa_key_derivation_operation_init(void)
{
    const struct psa_key_derivation_operation_s v = PSA_KEY_DERIVATION_OPERATION_INIT;

    return v;
}
#endif /* MODULE_PSA_KEY_DERIVATION */

#ifdef __cplusplus
}
#endif

/** @} */
