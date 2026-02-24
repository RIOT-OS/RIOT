/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        aead/algorithm.h
 * @brief       AEAD algorithm definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/algorithm.h"

/**
 * @brief   Category for AEAD algorithms.
 */
#define PSA_ALG_CATEGORY_AEAD ((psa_algorithm_t)0x05000000)

/**
 * @brief   Whether the specified algorithm is an authenticated encryption with
 *          associated data (AEAD) algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an AEAD algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_AEAD(alg) (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_AEAD)

/**
 * @brief   Whether the specified algorithm is an AEAD mode on a block cipher.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an AEAD algorithm which is an AEAD mode based on a block cipher
 *          0 otherwise.
 */
#define PSA_ALG_IS_AEAD_ON_BLOCK_CIPHER(alg) (((alg) & 0x7f400000) == 0x05400000)

/**
 * @brief   The Counter with CBC-MAC (CCM) authenticated encryption algorithm.
 *
 * @details CCM is defined for block ciphers that have a 128-bit block size. The underlying block
 *          cipher is determined by the key type.
 *
 *          To use @ref PSA_ALG_CCM with a multi-part AEAD operation, the application must call
 *          @ref psa_aead_set_lengths() before providing the nonce, the additional data and
 *          plaintext to the operation.
 *
 *          CCM requires a nonce of between 7 and 13 bytes in length. The length of the nonce
 *          affects the `m` maximum length of the plaintext than can be encrypted or decrypted. If
 *          the nonce has length `N`, then the plaintext length `pLen` is encoded in `L = 15 - N`
 *          octets, this requires that `pLen < 28L`.
 *
 *          The value for L that is used with PSA_ALG_CCM depends on the function used to provide
 *          the nonce:
 *           -  A call to @ref psa_aead_encrypt(), @ref psa_aead_decrypt(), or @ref
 *              psa_aead_set_nonce() will set `L` to 15 - nonce_length. If the plaintext length
 *              cannot be encoded in `L` octets, then a @ref PSA_ERROR_INVALID_ARGUMENT error is
 *              returned.
 *           -  A call to @ref psa_aead_generate_nonce() on a multi-part cipher operation will
 *              select `L` as the smallest integer `>= 2` where `pLen < 28L`, with `pLen` being the
 *              plaintext_length provided to @ref psa_aead_set_lengths(). The call to @ref
 *              psa_aead_generate_nonce() will generate and return a random nonce of length
 *              `15 - L` bytes.
 *
 *          CCM supports authentication tag sizes of 4, 6, 8, 10, 12, 14, and 16 bytes. The default
 *          tag length is 16. Shortened tag lengths can be requested using
 *          @ref PSA_ALG_AEAD_WITH_SHORTENED_TAG(@ref PSA_ALG_CCM, @p tag_length),
 *          where @c tag_length is a valid CCM tag length.
 *
 *          The CCM block cipher mode is defined in Counter with CBC-MAC (CCM)
 *          [RFC3610](https://tools.ietf.org/html/rfc3610).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_CCM ((psa_algorithm_t)0x05500100)

/**
 * @brief   The Galois/Counter Mode (GCM) authenticated encryption algorithm.
 *
 * @details GCM is defined for block ciphers that have a 128-bit block size. The underlying block
 *          cipher is determined by the key type.
 *
 *          GCM requires a nonce of at least 1 byte in length. The maximum supported nonce size is
 *          implementation defined. Calling @ref psa_aead_generate_nonce() will generate a random
 *          12-byte nonce.
 *
 *          GCM supports authentication tag sizes of 4, 8, 12, 13, 14, 15, and 16 bytes. The
 *          default tag length is 16. Shortened tag lengths can be requested using
 *          @ref PSA_ALG_AEAD_WITH_SHORTENED_TAG(@ref PSA_ALG_GCM, @p tag_length), where
 *          @c tag_length is a valid GCM tag length.
 *
 *          The GCM block cipher mode is defined in NIST Special Publication 800-38D:
 *          Recommendation for Block Cipher Modes of Operation: Galois/Counter Mode (GCM) and GMAC
 *          [SP800-38D](https://doi.org/10.6028/NIST.SP.800-38D).
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_AES
 *          - @ref PSA_KEY_TYPE_ARIA
 *          - @ref PSA_KEY_TYPE_CAMELLIA
 *          - @ref PSA_KEY_TYPE_SM4
 */
#define PSA_ALG_GCM ((psa_algorithm_t)0x05500200)

/**
 * @brief   The ChaCha20-Poly1305 AEAD algorithm.
 *
 * @details There are two defined variants of ChaCha20-Poly1305:
 *          -   An implementation that supports ChaCha20-Poly1305 must support the variant defined
 *              by ChaCha20 and Poly1305 for IETF Protocols
 *              [RFC7539](https://tools.ietf.org/html/rfc7539.html), which has a 96-bit nonce and
 *              32-bit counter.
 *          -   An implementation can optionally also support the original variant defined by
 *              ChaCha, a variant of Salsa20
 *              [CHACHA20](http://cr.yp.to/chacha/chacha-20080128.pdf), which has a 64-bit nonce
 *              and 64-bit counter.
 *
 *          The variant used for the AEAD encryption or decryption operation, depends on the nonce
 *          provided for an AEAD operation using @ref PSA_ALG_CHACHA20_POLY1305 :
 *          -   A nonce provided in a call to @ref psa_aead_encrypt(), @ref psa_aead_decrypt() or
 *              @ref psa_aead_set_nonce() must be 8 or 12 bytes. The size of nonce will select the
 *              appropriate variant of the algorithm.
 *          -   A nonce generated by a call to @ref psa_aead_generate_nonce() will be 12 bytes, and
 *              will use the [RFC7539](https://tools.ietf.org/html/rfc7539.html) variant.
 *
 *          Implementations must support 16-byte tags. It is recommended that truncated tag sizes
 *          are rejected.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_CHACHA20
 */
#define PSA_ALG_CHACHA20_POLY1305 ((psa_algorithm_t)0x05100500)

/**
 * @brief   Macro to build a AEAD algorithm with a shortened tag.
 *
 * @details An AEAD algorithm with a shortened tag is similar to the corresponding AEAD algorithm,
 *          but has an authentication tag that consists of fewer bytes. Depending on the algorithm,
 *          the tag length might affect the calculation of the ciphertext.
 *
 *          The AEAD algorithm with a default length tag can be recovered using
 *          @ref PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG().
 *
 *          @b Compatible @b key @b types:
 *          The resulting AEAD algorithm is compatible with the same key types as the AEAD
 *          algorithm used to construct it.
 *
 * @param aead_alg      An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p aead_alg) is true.
 * @param tag_length    Desired length of the authentication tag in bytes.
 *
 * @return  The corresponding AEAD algorithm with the specified tag length.
 *          Unspecified if @c aead_alg is not a supported AEAD algorithm or if @c tag_length is not
 *          valid for the specified AEAD algorithm.
 */
#define PSA_ALG_AEAD_WITH_SHORTENED_TAG(aead_alg, tag_length) \
    ((psa_algorithm_t)(((aead_alg) & ~0x003f0000) | (((tag_length) & 0x3f) << 16)))

/**
 * @brief   An AEAD algorithm with the default tag length.
 *
 * @details This macro can be used to construct the AEAD algorithm with default tag length from an
 *          AEAD algorithm with a shortened tag. See also @ref PSA_ALG_AEAD_WITH_SHORTENED_TAG().
 *
 *          @b Compatible @b key @b types:
 *          The resulting AEAD algorithm is compatible with the same key types as the
 *          AEAD algorithm used to construct it.
 */
#define PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(aead_alg) \
    ((((aead_alg) & ~0x003f0000) == 0x05400100) ? PSA_ALG_CCM : \
     (((aead_alg) & ~0x003f0000) == 0x05400200) ? PSA_ALG_GCM : \
     (((aead_alg) & ~0x003f0000) == 0x05000500) ? PSA_ALG_CHACHA20_POLY1305 : \
     PSA_ALG_NONE)

/**
 * @brief   Macro to build an AEAD minimum-tag-length wildcard algorithm.
 *
 * @details A key with a minimum-tag-length AEAD wildcard algorithm as permitted algorithm policy
 *          can be used with all AEAD algorithms sharing the same base algorithm, and where the tag
 *          length of the specific algorithm is equal to or larger then the minimum tag length
 *          specified by the wildcard algorithm.
 *
 * @note    When setting the minimum required tag length to less than the smallest tag length
 *          allowed by the base algorithm, this effectively becomes an ‘any-tag-length-allowed’
 *          policy for that base algorithm.
 *
 *          The AEAD algorithm with a default length tag can be recovered using
 *          @ref PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG().
 *
 * @param   aead_alg        An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                          @ref PSA_ALG_IS_AEAD(@p aead_alg) is true.
 * @param   min_tag_length  Desired minimum length of the authentication tag in bytes. This must be
 *                          at least 1 and at most the largest allowed tag length of the algorithm.
 *
 * @return  The corresponding AEAD wildcard algorithm with the specified minimum tag length
 *          Unspecified if @c aead_alg is not a supported AEAD algorithm or if @c min_tag_length is
 *          less than 1 or too large for the specified AEAD algorithm.
 */
#define PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(aead_alg, min_tag_length) \
    /* specification-defined value */

#ifdef __cplusplus
}
#endif

/** @} */
