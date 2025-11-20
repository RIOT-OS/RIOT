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

#if IS_USED(MODULE_PSA_KEY_MANAGEMENT) || defined(DOXYGEN)
/**
 * @brief   Structure storing the key usage policies
 */
struct psa_key_policy_s {
    psa_key_usage_t usage;  /**< Key usage policy */
    psa_algorithm_t alg;    /**< Algorithm for key usage */
};

/**
 * @brief   Type for key usage policies.
 */
typedef struct psa_key_policy_s psa_key_policy_t;

/**
 * @brief   Structure storing key attributes
 */
struct psa_key_attributes_s {
    psa_key_type_t type;            /**< Type of key */
    psa_key_bits_t bits;            /**< Size of key in bits */
    psa_key_lifetime_t lifetime;    /**< Lifetime of key */
    psa_key_id_t id;                /**< Key identifier */
    psa_key_policy_t policy;        /**< Key usage policy */
};

/**
 * @brief   This macro returns a suitable initializer for a key attribute object of
 *          type @ref psa_key_attributes_t.
 */
#define PSA_KEY_ATTRIBUTES_INIT { 0 }

/**
 * @brief Return an initial value for a key attribute object.
 *
 * @return struct psa_key_attributes_s
 */
static inline struct psa_key_attributes_s psa_key_attributes_init(void)
{
    const struct psa_key_attributes_s v = PSA_KEY_ATTRIBUTES_INIT;

    return v;
}
#endif /*MODULE_PSA_KEY_MANAGEMENT */

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

#if IS_USED(MODULE_PSA_CIPHER) || defined(DOXYGEN)

/**
 * @brief   Enum encoding available cipher operations.
 *
 * @details To be expanded with the development of this implementation.
 */
typedef enum {
    PSA_CBC_NO_PAD_AES_128,
    PSA_CBC_NO_PAD_AES_192,
    PSA_CBC_NO_PAD_AES_256,
    PSA_CBC_PKCS7_AES_256,
    PSA_STREAM_CIPHER_CHACHA20
} psa_cipher_op_t;

/**
 * @brief   Structure storing a cipher operation context
 */
struct psa_cipher_operation_s {
    uint8_t iv_required : 1;            /**< True if algorithm requires IV */
    uint8_t iv_set : 1;                 /**< True if IV was already set */
    uint8_t default_iv_length;          /**< Default IV length for algorithm */
    /** Combination of the psa_algorithm_t and psa_key_type_t for a specific implementation. */
    psa_cipher_op_t cipher_instance;
    /** Union containing cipher contexts for the executing backend */
    union cipher_context {
        psa_cipher_context_t cipher_ctx;    /**< Cipher context */
#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A) || defined(DOXYGEN)
        psa_se_cipher_context_t se_ctx;     /**< SE Cipher context */
#endif
    } backend_ctx;  /**< Backend specific cipher context */
};

/**
 * @brief   This macro returns a suitable initializer for a cipher operation
 *          object of type @ref psa_cipher_operation_t.
 */
#define PSA_CIPHER_OPERATION_INIT { 0 }

/**
 * @brief   Return an initial value for a cipher operation object.
 *
 * @return  psa_cipher_operation_s
 */
static inline struct psa_cipher_operation_s psa_cipher_operation_init(void)
{
    const struct psa_cipher_operation_s v = PSA_CIPHER_OPERATION_INIT;

    return v;
}
#endif /* MODULE_PSA_CIPHER */

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

#if IS_USED(MODULE_PSA_HASH) || defined(DOXYGEN)
/**
 * @brief   Structure containing a hash context and algorithm
 */
struct psa_hash_operation_s {
    psa_algorithm_t alg;        /**< Operation algorithm */
#if IS_USED(MODULE_PSA_HASH)
    psa_hash_context_t ctx;     /**< Operation hash context */
#endif
};

/**
 * @brief   This macro returns a suitable initializer for a hash operation object of type
 *          @ref psa_hash_operation_t.
 */
#define PSA_HASH_OPERATION_INIT { 0 }

/**
 * @brief   Return an initial value for a hash operation object.
 *
 * @return  struct psa_hash_operation_s
 */
static inline struct psa_hash_operation_s psa_hash_operation_init(void)
{
    const struct psa_hash_operation_s v = PSA_HASH_OPERATION_INIT;

    return v;
}
#endif /* MODULE_PSA_HASH */

#if IS_USED(MODULE_PSA_MAC) || defined(DOXYGEN)
/**
 * @brief   This macro returns a suitable initializer for a MAC operation object of type
 *          @ref psa_mac_operation_t.
 */
#define PSA_MAC_OPERATION_INIT { 0 }

/**
 * @brief   Structure storing a MAC operation context
 *
 * @note    Not yet implemented
 */
struct psa_mac_operation_s {
    int dummy;  /**< Not yet implemented */
};

/**
 * @brief   Return an initial value for a MAC operation object.
 *
 * @return  psa_mac_operation_s
 */
static inline struct psa_mac_operation_s psa_mac_operation_init(void)
{
    const struct psa_mac_operation_s v = PSA_MAC_OPERATION_INIT;

    return v;
}
#endif /* MODULE_PSA_MAC */

#ifdef __cplusplus
}
#endif

/** @} */
