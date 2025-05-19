/*
 * Copyright (C) 2024 TU Dresden
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
 * @file        hash/algorithm.h
 * @brief       Hash algorithm definitions for the PSA Crypto API
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
 * @brief   Category for hash algorithms.
 */
#define PSA_ALG_CATEGORY_HASH ((psa_algorithm_t)0x02000000)

/**
 * @brief   Hash algorithm mask.
 */
#define PSA_ALG_HASH_MASK ((psa_algorithm_t)0x000000ff)

/**
 * @brief   Whether the specified algorithm is a hash algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a hash algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HASH(alg) (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_HASH)

/**
 * @brief   Get the hash used by a composite algorithm.
 *
 *          The following composite algorithms require a hash algorithm:
 *          - @ref PSA_ALG_ECDSA()
 *          - @ref PSA_ALG_HKDF()
 *          - @ref PSA_ALG_HMAC()
 *          - @ref PSA_ALG_RSA_OAEP()
 *          - @ref PSA_ALG_IS_RSA_PKCS1V15_SIGN()
 *          - @ref PSA_ALG_RSA_PSS()
 *          - @ref PSA_ALG_RSA_PSS_ANY_SALT()
 *          - @ref PSA_ALG_TLS12_PRF()
 *          - @ref PSA_ALG_TLS12_PSK_TO_MS()
 *          - @ref PSA_ALG_PBKDF2_HMAC()
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  The underlying hash algorithm if alg is a composite algorithm that uses a hash algorithm
 *          @ref PSA_ALG_NONE if alg is not a composite algorithm that uses a hash.
 */
#define PSA_ALG_GET_HASH(alg) \
    (((alg) & PSA_ALG_HASH_MASK) == \
     0 ? ((psa_algorithm_t)0) : PSA_ALG_CATEGORY_HASH | ((alg) & PSA_ALG_HASH_MASK))

/**
 * @brief   Get the hash operation of a hmac algorithm
 *
 * @param   hmac_alg    HMAC algorithm
 */
#define PSA_ALG_HMAC_GET_HASH(hmac_alg) \
    (PSA_ALG_CATEGORY_HASH | ((hmac_alg) & PSA_ALG_HASH_MASK))

/**
 * @brief   The MD2 message-digest algorithm.
 *
 * @warning The MD2 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details MD2 is defined in *The MD2 Message-Digest Algorithm
 *          [RFC1319](https://tools.ietf.org/html/rfc1319.html)*.
 */
#define PSA_ALG_MD2 ((psa_algorithm_t)0x02000001)

/**
 * @brief   The MD4 message-digest algorithm.
 *
 * @warning The MD4 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details MD4 is defined in *The MD4 Message-Digest Algorithm
 *          [RFC1320](https://tools.ietf.org/html/rfc1320.html)*.
 */
#define PSA_ALG_MD4 ((psa_algorithm_t)0x02000002)

/**
 * @brief   The MD5 message-digest algorithm.
 *
 * @warning The MD5 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details MD5 is defined in *The MD5 Message-Digest Algorithm
 *          [RFC1321](https://tools.ietf.org/html/rfc1321.html)*.
 */
#define PSA_ALG_MD5 ((psa_algorithm_t)0x02000003)

/**
 * @brief   The RIPEMD-160 message-digest algorithm.
 *
 * @details RIPEMD-160 is defined in *RIPEMD-160: A Strengthened Version of RIPEMD*,
 *          and also in *ISO/IEC 10118-3:2018 IT Security techniques — Hash-functions — Part 3:
 *          Dedicated hash-functions [ISO10118](https://www.iso.org/standard/67116.html)*.
 */
#define PSA_ALG_RIPEMD160 ((psa_algorithm_t)0x02000004)

/**
 * @brief   The SHA-1 message-digest algorithm.
 *
 * @warning The SHA-1 hash is weak and deprecated and is only recommended
 *          for use in legacy protocols.
 *
 * @details SHA-1 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_1 ((psa_algorithm_t)0x02000005)

/**
 * @brief   The SHA-224 message-digest algorithm.
 *
 * @details SHA-224 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_224 ((psa_algorithm_t)0x02000008) /** SHA-224 */

/**
 * @brief   The SHA-256 message-digest algorithm.
 *
 * @details SHA-256 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_256 ((psa_algorithm_t)0x02000009) /** SHA-256 */

/**
 * @brief   The SHA-384 message-digest algorithm.
 *
 * @details SHA-384 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_384 ((psa_algorithm_t)0x0200000a) /** SHA-384 */

/**
 * @brief   The SHA-512 message-digest algorithm.
 *
 * @details SHA-512 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_512 ((psa_algorithm_t)0x0200000b) /** SHA-512 */

/**
 * @brief   The SHA-512/224 message-digest algorithm.
 *
 * @details SHA-512-224 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_512_224 ((psa_algorithm_t)0x0200000c) /** SHA-512/224 */

/**
 * @brief   The SHA-512/256 message-digest algorithm.
 *
 * @details SHA-512-256 is defined in FIPS Publication 180-4: Secure Hash Standard (SHS)
 *          [FIPS180-4](https://doi.org/10.6028/NIST.FIPS.180-4).
 */
#define PSA_ALG_SHA_512_256 ((psa_algorithm_t)0x0200000d) /** SHA-512/256 */

/**
 * @brief   The SHA3-224 message-digest algorithm.
 *
 * @details SHA3-224 is defined in FIPS Publication 202: *SHA-3 Standard: Permutation-Based
 *          Hash and Extendable-Output Functions [FIPS202](https://doi.org/10.6028/NIST.FIPS.202)*.
 */
#define PSA_ALG_SHA3_224 ((psa_algorithm_t)0x02000010) /** SHA-3-224 */

/**
 * @brief   The SHA3-256 message-digest algorithm.
 *
 * @details SHA3-256 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 */
#define PSA_ALG_SHA3_256 ((psa_algorithm_t)0x02000011) /** SHA-3-256 */

/**
 * @brief   The SHA3-384 message-digest algorithm.
 *
 * @details SHA3-384 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 */
#define PSA_ALG_SHA3_384 ((psa_algorithm_t)0x02000012) /** SHA-3-384 */

/**
 * @brief   The SHA3-512 message-digest algorithm.
 *
 * @details SHA3-512 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 */
#define PSA_ALG_SHA3_512 ((psa_algorithm_t)0x02000013) /** SHA-3-512 */

/**
 * @brief   The SM3 message-digest algorithm.
 *
 * @details SM3 is defined in *ISO/IEC 10118-3:2018 IT Security techniques — Hash-functions —
 *          Part 3: Dedicated hash-functions [ISO10118](https://www.iso.org/standard/67116.html)*,
 *          and also in *GM/T 0004-2012: SM3 cryptographic hash algorithm
 *          [CSTC0004](http://www.gmbz.org.cn/main/viewfile/20180108023812835219.html)
 *          (English version
 *          [CSTC0004/E](http://www.gmbz.org.cn/main/postDetail.html?id=20180724105928))*.
 */
#define PSA_ALG_SM3 ((psa_algorithm_t)0x02000014) /** SM3 */

/**
 * @brief   The first 512 bits (64 bytes) of the SHAKE256 output.
 *
 * @details This is the prehashing for Ed448ph (see @ref PSA_ALG_ED448PH).
 *
 *          SHAKE256 is defined in [FIPS202](https://doi.org/10.6028/NIST.FIPS.202).
 *
 * @note    For other scenarios where a hash function based on SHA3 or SHAKE is required, SHA3-512
 *          is recommended. SHA3-512 has the same output size, and a theoretically higher security
 *          strength.
 */
#define PSA_ALG_SHAKE256_512 ((psa_algorithm_t)0x02000015)

#ifdef __cplusplus
}
#endif

/** @} */
