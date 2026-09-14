/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file
 * @brief       Hash type definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "hashes/psa/riot_hashes.h"
#include "kernel_defines.h"
#include "psa/algorithm.h"

#if IS_USED(MODULE_PSA_RIOT_HASHES_HMAC_SHA256) || IS_USED(MODULE_PSA_RIOT_HASHES_MD5) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_1) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA_224) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_256) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA_384) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_512) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA_512_224) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_512_256) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA3_256) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA3_384) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA3_512)
#include "hashes/psa/riot_hashes.h"
#endif

#if IS_USED(MODULE_PERIPH_HASH_SHA_1) || IS_USED(MODULE_PERIPH_HASH_SHA_224) || \
    IS_USED(MODULE_PERIPH_HASH_SHA_256) || IS_USED(MODULE_PERIPH_HASH_SHA_384) || \
    IS_USED(MODULE_PERIPH_HASH_SHA_512) || IS_USED(MODULE_PERIPH_HASH_SHA_512_224) || \
    IS_USED(MODULE_PERIPH_HASH_SHA_512_256)
#include "psa_periph_hashes_ctx.h"
#endif

/**
 * @brief   Structure containing the hash contexts needed by the application.
 */
typedef union {
#if IS_USED(MODULE_PSA_HASH_MD5) || defined(DOXYGEN)
    psa_hashes_md5_ctx_t md5;   /**< MD5 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_1) || defined(DOXYGEN)
    psa_hashes_sha1_ctx_t sha1; /**< SHA-1 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_224) || defined(DOXYGEN)
    psa_hashes_sha224_ctx_t sha224; /**< SHA-224 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_256) || defined(DOXYGEN)
    psa_hashes_sha256_ctx_t sha256; /**< SHA-256 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_384) || defined(DOXYGEN)
    psa_hashes_sha384_ctx_t sha384; /**< SHA-384 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_512) || defined(DOXYGEN)
    psa_hashes_sha512_ctx_t sha512; /**< SHA-512 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA3_256) || IS_USED(MODULE_PSA_HASH_SHA3_384) \
|| IS_USED(MODULE_PSA_HASH_SHA3_512) || defined(DOXYGEN)
    psa_hashes_sha3_ctx_t sha3; /**< SHA-3 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_512_224) || defined(DOXYGEN)
    psa_hashes_sha512_224_ctx_t sha512_224; /**< SHA-512/224 context */
#endif
#if IS_USED(MODULE_PSA_HASH_SHA_512_256) || defined(DOXYGEN)
    psa_hashes_sha512_256_ctx_t sha512_256; /**< SHA-512/256 context */
#endif
} psa_hash_context_t;

/**
 * @brief   Structure containing a hash context and algorithm
 */
struct psa_hash_operation_s {
    psa_algorithm_t alg;        /**< Operation algorithm */
#if IS_USED(MODULE_PSA_HASH) || defined(DOXYGEN)
    psa_hash_context_t ctx;     /**< Operation hash context */
#endif
};

/**
 * @brief   The type of the state data structure for multipart hash operations.
 *
 * @details Before calling any function on a hash operation object, the application must
 *          initialize it by any of the following means:
 *          - Set the structure to all-bits-zero, for example:
 *            @code
 *            @ref psa_hash_operation_t operation;
 *            memset(&operation, 0, sizeof(operation));
 *            @endcode
 *          - Initialize the structure to logical zero values, for example:
 *            @code
 *            @ref psa_hash_operation_t operation = {0};
 *            @endcode
 *          - Initialize the structure to the initializer @ref PSA_HASH_OPERATION_INIT,
 *            for example:
 *            @code
 *            @ref psa_hash_operation_t operation = @ref PSA_HASH_OPERATION_INIT;
 *            @endcode
 *          - Assign the result of the function @ref psa_hash_operation_init()
 *            to the structure, for example:
 *            @code
 *            @ref psa_hash_operation_t operation;
 *            operation = @ref psa_hash_operation_init();
 *            @endcode
 *
 *          This is an implementation-defined struct. Applications should not
 *          make any assumptions about the content of this structure except
 *          as directed by the documentation of a specific implementation.
 */
typedef struct psa_hash_operation_s psa_hash_operation_t;

/**
 * @brief   This macro returns a suitable initializer for a hash operation object of type
 *          @ref psa_hash_operation_t.
 */
#define PSA_HASH_OPERATION_INIT { 0 }

/**
 * @brief   Return an initial value for a hash operation object.
 *
 * @return  struct psa_hash_operation_t
 */
static inline psa_hash_operation_t psa_hash_operation_init(void)
{
    const psa_hash_operation_t v = PSA_HASH_OPERATION_INIT;

    return v;
}

#ifdef __cplusplus
}
#endif

/** @} */
