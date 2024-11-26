/*
 * Copyright (C) 2024 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        mac/algorithm.h
 * @brief       MAC algorithm definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_MAC_ALGORITHM_H
#define PSA_CRYPTO_PSA_MAC_ALGORITHM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/algorithm.h"
#include "psa/hash/algorithm.h"

/**
 * @brief   Category for MAC algorithms.
 */
#define PSA_ALG_CATEGORY_MAC ((psa_algorithm_t)0x03000000)

/**
 * @brief   HMAC Base.
 */
#define PSA_ALG_HMAC_BASE (0x03800000)

/**
 * @brief   Whether the specified algorithm is a MAC algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a MAC algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_MAC(alg) (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_MAC)

/**
 * @brief   Whether the specified algorithm is an HMAC algorithm.
 *
 * @details HMAC is a family of MAC algorithms that are based on a hash function.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an HMAC algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HMAC(alg) (((alg) & 0x7fc0ff00) == 0x03800000)

/**
 * @brief   Whether the specified algorithm is a MAC algorithm based on a block cipher.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a MAC algorithm based on a block cipher
 *          0 otherwise
 */
#define PSA_ALG_IS_BLOCK_CIPHER_MAC(alg) (((alg) & 0x7fc00000) == 0x03c00000)

/**
 * @brief   Macro to build an HMAC message-authentication-code algorithm from an underlying hash
 *          algorithm.
 *
 * @details For example, @ref PSA_ALG_HMAC(@ref PSA_ALG_SHA_256) is HMAC-SHA-256.
 *          The HMAC construction is defined in HMAC: Keyed-Hashing for Message Authentication
 *          [RFC2104].
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_HMAC
 *
 * @param   hash_alg    A hash algorithm (PSA_ALG_XXX value such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true).
 *
 * @return  The corresponding HMAC algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_HMAC(hash_alg) \
    ((psa_algorithm_t)(PSA_ALG_HMAC_BASE | ((hash_alg) & PSA_ALG_HASH_MASK)))

/**
 * @brief   The CBC-MAC message-authentication-code algorithm, constructed over a block cipher.
 *
 * @warning CBC-MAC is insecure in many cases. A more secure mode, such as @ref PSA_ALG_CMAC,
 *          is recommended.
 *
 * @details The CBC-MAC algorithm must be used with a key for a block cipher.
 *          For example, one of @ref PSA_KEY_TYPE_AES.
 *
 *          CBC-MAC is defined as MAC Algorithm 1 in ISO/IEC 9797-1:2011 Information technology —
 *          Security techniques — Message Authentication Codes (MACs) — Part 1: Mechanisms using a
 *          block cipher
 *          [ISO9797](https://www.iso.org/standard/50375.html).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CBC_MAC ((psa_algorithm_t)0x03c00100)

/**
 * @brief   The CMAC message-authentication-code algorithm, constructed over a block cipher.
 *
 * @details The CMAC algorithm must be used with a key for a block cipher. For example, when used
 *          with a key with type @ref PSA_KEY_TYPE_AES, the resulting operation is AES-CMAC.
 *
 *          CMAC is defined in NIST Special Publication 800-38B: Recommendation for Block Cipher
 *          Modes of Operation: the CMAC Mode for Authentication
 *          [SP800-38B](https://doi.org/10.6028/NIST.SP.800-38B).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_DES
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CMAC ((psa_algorithm_t)0x03c00200)

/**
 * @brief   Macro to build a truncated MAC algorithm.
 *
 * @details A truncated MAC algorithm is identical to the corresponding MAC algorithm except that
 *          the MAC value for the truncated algorithm consists of only the first mac_length bytes
 *          of the MAC value for the untruncated algorithm.
 *
 * @note    This macro might allow constructing algorithm identifiers that are not valid, either
 *          because the specified length is larger than the untruncated MAC or because the
 *          specified length is smaller than permitted by the implementation.
 *
 * @note    It is implementation-defined whether a truncated MAC that is truncated to the same
 *          length as the MAC of the untruncated algorithm is considered identical to the
 *          untruncated algorithm for policy comparison purposes.
 *
 *          The untruncated MAC algorithm can be recovered using @ref PSA_ALG_FULL_LENGTH_MAC().
 *
 *          @b Compatible @b key @b types
 *          The resulting truncated MAC algorithm is compatible with the same key types as the MAC
 *          algorithm used to construct it.
 *
 * @param   mac_alg     A MAC algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_MAC(@p mac_alg) is true. This can be a truncated or
 *                      untruncated MAC algorithm.
 * @param   mac_length  Desired length of the truncated MAC in bytes. This must be at most the
 *                      untruncated length of the MAC and must be at least an
 *                      implementation-specified minimum. The implementation-specified minimum must
 *                      not be zero.
 *
 * @return  The corresponding MAC algorithm with the specified length.
 *          Unspecified if @c mac_alg is not a supported MAC algorithm or if @c mac_length is too
 *          small or too large for the specified MAC algorithm.
 */
#define PSA_ALG_TRUNCATED_MAC(mac_alg, mac_length) \
    ((psa_algorithm_t)(((mac_alg) & ~0x003f0000) | (((mac_length) & 0x3f) << 16)))

/**
 * @brief   Macro to construct the MAC algorithm with a full length MAC, from a truncated MAC
 *          algorithm.
 *
 * @details @b Compatible @b key @b types
 *          The resulting untruncated MAC algorithm is compatible with the same key types as the MAC
 *          algorithm used to construct it.
 *
 * @param   mac_alg A MAC algorithm: a value of type @ref psa_algorithm_t such that
 *                  @ref PSA_ALG_IS_MAC(@p mac_alg) is true.
 *                  This can be a truncated or untruncated MAC algorithm.
 *
 * @return  The corresponding MAC algorithm with a full length MAC.
 *          Unspecified if @c mac_alg is not a supported MAC algorithm. *
 */
#define PSA_ALG_FULL_LENGTH_MAC(mac_alg) \
    ((psa_algorithm_t)((mac_alg) & ~0x003f0000))

/**
 * @brief   Macro to build a MAC minimum-MAC-length wildcard algorithm.
 *
 * @details A key with a minimum-MAC-length MAC wildcard algorithm as permitted algorithm policy
 *          can be used with all MAC algorithms sharing the same base algorithm, and where the
 *          (potentially truncated) MAC length of the specific algorithm is equal to or larger then
 *          the wildcard algorithm’s minimum MAC length.
 *
 * @note    When setting the minimum required MAC length to less than the smallest MAC length
 *          allowed by the base algorithm, this effectively becomes an ‘any-MAC-length-allowed’
 *          policy for that base algorithm.
 *
 *          The untruncated MAC algorithm can be recovered using @ref PSA_ALG_FULL_LENGTH_MAC().
 *
 *          @b Compatible @b key @b types
 *          The resulting wildcard MAC algorithm is compatible with the same key types as the MAC
 *          algorithm used to construct it.
 *
 * @param   mac_alg         A MAC algorithm: a value of type psa_algorithm_t such that
 *                          @ref PSA_ALG_IS_MAC(alg) is true.
 *                          This can be a truncated or untruncated MAC algorithm.
 * @param   min_mac_length  Desired minimum length of the message authentication code in bytes.
 *                          This must be at most the untruncated length of the MAC and must be at
 *                          least 1.
 * @return  The corresponding MAC wildcard algorithm with the specified minimum MAC length.
 *          Unspecified if mac_alg is not a supported MAC algorithm or if min_mac_length is less
 *          than 1 or too large for the specified MAC algorithm.
 */
#define PSA_ALG_AT_LEAST_THIS_LENGTH_MAC(mac_alg, min_mac_length) \
    /* specification-defined value */

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_MAC_ALGORITHM_H */
/** @} */
