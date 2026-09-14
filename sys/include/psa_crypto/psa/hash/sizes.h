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
 * @brief       Hash size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/hash/algorithm.h"

/**
 * @brief   The size of the output of @ref psa_hash_compute() and @ref psa_hash_finish(), in bytes.
 *
 * @details This is also the hash length that @ref psa_hash_compare() and @ref psa_hash_verify()
 *          expect.
 *
 *          See also @ref PSA_HASH_MAX_SIZE.
 *
 * @param   alg A hash algorithm or an HMAC algorithm: a value of type @ref psa_algorithm_t such
 *              that (@ref PSA_ALG_IS_HASH(@p alg) || @ref PSA_ALG_IS_HMAC(@p alg)) is true.
 *
 * @return  The hash length for the specified hash algorithm.
 *          0 if the hash algorithm is not recognized or not supported.
 */
#define PSA_HASH_LENGTH(alg)                                        \
    (                                                               \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD2 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD4 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD5 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_RIPEMD160 ? 20 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_1 ? 20 :          \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_224 ? 28 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_256 ? 32 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_384 ? 48 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_224 ? 28 :    \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_256 ? 32 :    \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_224 ? 28 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_256 ? 32 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_384 ? 48 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_512 ? 64 :       \
        0)

/**
 * @brief   Maximum size of a hash supported by this implementation, in bytes.
 *
 *          See also @ref PSA_HASH_LENGTH().
 */
#define PSA_HASH_MAX_SIZE   (64)

/**
 * @brief   Maximum size of a hash block supported by this implementation, in bytes.
 *
 *          See also @ref PSA_HASH_BLOCK_LENGTH().
 */
#if   (IS_USED(MODULE_PSA_HASH_SHA3_256))
#define PSA_HASH_MAX_BLOCK_SIZE 136
#elif (IS_USED(MODULE_PSA_HASH_SHA_512) || \
       IS_USED(MODULE_PSA_HASH_SHA_384) || \
       IS_USED(MODULE_PSA_HASH_SHA_512_224) || \
       IS_USED(MODULE_PSA_HASH_SHA_512_256))
#define PSA_HASH_MAX_BLOCK_SIZE 128
#elif (IS_USED(MODULE_PSA_HASH_SHA3_384))
#define PSA_HASH_MAX_BLOCK_SIZE 104
#elif (IS_USED(MODULE_PSA_HASH_SHA3_512))
#define PSA_HASH_MAX_BLOCK_SIZE 72
#elif (IS_USED(MODULE_PSA_HASH_MD5) || \
       IS_USED(MODULE_PSA_HASH_SHA_1) || \
       IS_USED(MODULE_PSA_HASH_SHA_224) || \
       IS_USED(MODULE_PSA_HASH_SHA_256))
#define PSA_HASH_MAX_BLOCK_SIZE 64
#else
#define PSA_HASH_MAX_BLOCK_SIZE 0
#endif

/**
 * @brief   A sufficient hash suspend state buffer size for @ref psa_hash_suspend(), in bytes.
 *
 * @details If the size of the hash state buffer is at least this large, it is guaranteed that
 *          @ref psa_hash_suspend() will not fail due to an insufficient buffer size. The actual
 *          size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE.
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(alg) is true.
 *
 * @return  A sufficient output size for the algorithm.
 *          0 if the hash algorithm is not recognized, or is not supported by
 *          @ref psa_hash_suspend().
 *
 *          For a supported hash algorithm alg, the following expression is true:
 *          @code
 *          PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) == PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH +
 *                                               PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) +
 *                                               PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) +
 *                                               PSA_HASH_BLOCK_LENGTH(alg) - 1
 *          @endcode
 */
#define PSA_HASH_SUSPEND_OUTPUT_SIZE(alg) /* specification-defined value */

/**
 * @brief   A sufficient hash suspend state buffer size for @ref psa_hash_suspend(),
 *          for any supported hash algorithms.
 *
 * @details If the size of the hash state buffer is at least this large, it is guaranteed that
 *          @ref psa_hash_suspend() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_HASH_SUSPEND_OUTPUT_SIZE().
 */
#define PSA_HASH_SUSPEND_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   The size of the algorithm field that is part of the output of @ref psa_hash_suspend(),
 *          in bytes.
 *
 * @details Applications can use this value to unpack the hash suspend state that is output by
 *          @ref psa_hash_suspend().
 */
#define PSA_HASH_SUSPEND_ALGORITHM_FIELD_LENGTH ((size_t)4)

/**
 * @brief   The size of the input-length field that is part of the output of
 *          @ref psa_hash_suspend(), in bytes.
 *
 * @details Applications can use this value to unpack the hash suspend state that is output
 *          by @ref psa_hash_suspend().
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  The size, in bytes, of the input-length field of the hash suspend state for the
 *          specified hash algorithm.
 *          0 i f the hash algorithm is not recognized or not supported.
 */
#define PSA_HASH_SUSPEND_INPUT_LENGTH_FIELD_LENGTH(alg) \
/* specification-defined value */

/**
 * @brief   The size of the hash-state field that is part of the output of @ref psa_hash_suspend(),
 *          in bytes.
 *
 * @details Applications can use this value to unpack the hash suspend state that is output by
 *          @ref psa_hash_suspend().
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  The size, in bytes, of the hash-state field of the hash suspend state for the specified
 *          hash algorithm.
 *          0 if the hash algorithm is not recognized or not supported.
 */
#define PSA_HASH_SUSPEND_HASH_STATE_FIELD_LENGTH(alg) \
/* specification-defined value */

/**
 * @brief   The input block size of a hash algorithm, in bytes.
 *
 * @details Hash algorithms process their input data in blocks. Hash operations will retain any
 *          partial blocks until they have enough input to fill the block or until the operation
 *          is finished.
 *
 *          This affects the output from @ref psa_hash_suspend().
 *
 * @param   alg A hash algorithm: a value of type @ref psa_algorithm_t such that
 *              @ref PSA_ALG_IS_HASH(@p alg) is true.
 *
 * @return  The block size in bytes for the specified hash algorithm. If the hash algorithm is not
 *          recognized, return 0. An implementation can return either 0 or the correct size for a
 *          hash algorithm that it recognizes, but does not support.
 */
#define PSA_HASH_BLOCK_LENGTH(alg)                                  \
    (                                                               \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD2 ? 16 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD4 ? 64 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_MD5 ? 64 :            \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_RIPEMD160 ? 64 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_1 ? 64 :          \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_224 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_256 ? 64 :        \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_384 ? 128 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512 ? 128 :       \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_224 ? 128 :   \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA_512_256 ? 128 :   \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_224 ? 144 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_256 ? 136 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_384 ? 104 :      \
        PSA_ALG_HMAC_GET_HASH(alg) == PSA_ALG_SHA3_512 ? 72 :       \
        0)

#ifdef __cplusplus
}
#endif

/** @} */
