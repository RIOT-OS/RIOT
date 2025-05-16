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
 * @file        key_derivation/algorithm.h
 * @brief       Key derivation algorithm definitions for the PSA Crypto API
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
 * @brief   Category for key derivation algorithms
 */
#define PSA_ALG_CATEGORY_KEY_DERIVATION ((psa_algorithm_t)0x08000000)

/**
 * @brief   Whether the specified algorithm is a key derivation algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a key derivation algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_KEY_DERIVATION(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_KEY_DERIVATION)

/**
 * @brief   Whether the specified algorithm is a key-stretching or password-hashing algorithm.
 *
 * @details A key-stretching or password-hashing algorithm is a key derivation algorithm that is
 *          suitable for use with a low-entropy secret such as a password. Equivalently, it’s a key
 *          derivation algorithm that uses a @ref PSA_KEY_DERIVATION_INPUT_PASSWORD input step.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a key-stretching or password-hashing algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_KEY_DERIVATION_STRETCHING(alg) \
    (((alg) & 0x7f800000) == 0x08800000)

/**
 * @brief   Whether the specified algorithm is an HKDF algorithm.
 *
 * @details HKDF is a family of key derivation algorithms that are based on a hash function and the
 *          HMAC construction.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an HKDF algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HKDF(alg) \
    (((alg) & ~0x000000ff) == 0x08000100)

/**
 * @brief   Whether the specified algorithm is an HKDF-Extract algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an HKDF-Extract algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HKDF_EXTRACT(alg) \
    (((alg) & ~0x000000ff) == 0x08000400)

/**
 * @brief   Whether the specified algorithm is an HKDF-Expand algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an HKDF-Expand algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_HKDF_EXPAND(alg) \
    (((alg) & ~0x000000ff) == 0x08000500)

/**
 * @brief   Whether the specified algorithm is a TLS-1.2 PRF algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a TLS-1.2 PRF algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_TLS12_PRF(alg) \
    (((alg) & ~0x000000ff) == 0x08000200)

/**
 * @brief   Whether the specified algorithm is a TLS-1.2 PSK to MS algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a TLS-1.2 PSK to MS algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_TLS12_PSK_TO_MS(alg) \
    (((alg) & ~0x000000ff) == 0x08000300)

/**
 * @brief   Whether the specified algorithm is a PBKDF2-HMAC algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a PBKDF2-HMAC algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_PBKDF2_HMAC(alg) \
    (((alg) & ~0x000000ff) == 0x08800100)

/**
 * @brief   Macro to build an HKDF algorithm.
 *
 * @details This is the HMAC-based Extract-and-Expand Key Derivation Function (HKDF) specified by
 *          HMAC-based Extract-and-Expand Key Derivation Function (HKDF) [RFC5869].
 *
 *          This key derivation algorithm uses the following inputs:
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SALT is the salt used in the “extract” step. It is
 *              optional; if omitted, the derivation uses an empty salt.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_INFO is the info string used in the “expand” step.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SECRET is the secret key used in the “extract” step.
 *
 *          If @ref PSA_KEY_DERIVATION_INPUT_SALT is provided, it must be before
 *          @ref PSA_KEY_DERIVATION_INPUT_SECRET. @ref PSA_KEY_DERIVATION_INPUT_INFO can be
 *          provided at any time after setup and before starting to generate output.
 *
 *          Each input may only be passed once.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the secret key)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the other inputs)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding HKDF algorithm. For example, @ref PSA_ALG_HKDF(@ref PSA_ALG_SHA_256)
 *          is HKDF using HMAC-SHA-256.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_HKDF(hash_alg) ((psa_algorithm_t)(0x08000100 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build an HKDF-Extract algorithm.
 *
 * @details This is the Extract step of HKDF as specified by
 *          HMAC-based Extract-and-Expand Key Derivation Function (HKDF) [RFC5869] §2.2.
 *
 *          This key derivation algorithm uses the following inputs:
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SALT is the salt.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SECRET is the input keying material used
 *              in the “extract” step.
 *
 *          The inputs are mandatory and must be passed in the order above.
 *          Each input may only be passed once.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the input keying material)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the salt)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding HKDF-Extract algorithm. For example,
 *          @ref PSA_ALG_HKDF_EXTRACT(@ref PSA_ALG_SHA_256) is HKDF-Extract using HMAC-SHA-256.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_HKDF_EXTRACT(hash_alg) ((psa_algorithm_t)(0x08000400 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build an HKDF-Expand algorithm.
 *
 * @details This is the Expand step of HKDF as specified by
 *          HMAC-based Extract-and-Expand Key Derivation Function (HKDF) [RFC5869] §2.3.
 *
 *          This key derivation algorithm uses the following inputs:
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SECRET is the pseudoramdom key (PRK).
 *          -   @ref PSA_KEY_DERIVATION_INPUT_INFO is the info string.
 *
 *          The inputs are mandatory and must be passed in the order above.
 *          Each input may only be passed once.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the pseudorandom key)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the info string)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding HKDF-Expand algorithm. For example,
 *          @ref PSA_ALG_HKDF_EXPAND(@ref PSA_ALG_SHA_256) is HKDF-Expand using HMAC-SHA-256.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_HKDF_EXPAND(hash_alg) ((psa_algorithm_t)(0x08000500 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build a TLS-1.2 PRF algorithm.
 *
 * @details TLS 1.2 uses a custom pseudorandom function (PRF) for key schedule, specified in
 *          The Transport Layer Security (TLS) Protocol Version 1.2
 *          [RFC5246](https://tools.ietf.org/html/rfc5246.html) §5. It is based on
 *          HMAC and can be used with either SHA-256 or SHA-384.
 *
 *          This key derivation algorithm uses the following inputs, which must be passed in the
 *          order given here:
 *          - @ref PSA_KEY_DERIVATION_INPUT_SEED is the seed.
 *          - @ref PSA_KEY_DERIVATION_INPUT_SECRET is the secret key.
 *          - @ref PSA_KEY_DERIVATION_INPUT_LABEL is the label.
 *
 *          Each input may only be passed once.
 *
 *          For the application to TLS-1.2 key expansion:
 *          - The seed is the concatenation of @c ServerHello.Random + @c ClientHello.Random.
 *          - The label is `key expansion`.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the secret key)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the other inputs)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding TLS-1.2 PRF algorithm. For example,
 *          @ref PSA_ALG_TLS12_PRF(@p PSA_ALG_SHA_256) represents the TLS 1.2 PRF using
 *          HMAC-SHA-256.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_TLS12_PRF(hash_alg) ((psa_algorithm_t)(0x08000200 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build a TLS-1.2 PSK-to-MasterSecret algorithm.
 *
 * @details In a pure-PSK handshake in TLS 1.2, the master secret (MS) is derived from the
 *          pre-shared key (PSK) through the application of padding (Pre-Shared Key Ciphersuites
 *          for Transport Layer Security (TLS) [RFC4279](https://tools.ietf.org/html/rfc4279.html)
 *          §2) and the TLS-1.2 PRF (The Transport Layer Security (TLS) Protocol Version 1.2
 *          [RFC5246](https://tools.ietf.org/html/rfc5246.html) §5). The latter is based on HMAC
 *          and can be used with either SHA-256 or SHA-384.
 *
 *          This key derivation algorithm uses the following inputs, which must be passed in the
 *          order given here:
 *          - @ref PSA_KEY_DERIVATION_INPUT_SEED is the seed.
 *          - @ref PSA_KEY_DERIVATION_INPUT_SECRET is the PSK. The PSK must not be larger than
 *            @ref PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE.
 *          - @ref PSA_KEY_DERIVATION_INPUT_LABEL is the label.
 *
 *          Each input may only be passed once.
 *
 *          For the application to TLS-1.2:
 *          -   The seed, which is forwarded to the TLS-1.2 PRF, is the concatenation of the
 *              @c ClientHello.Random + @c ServerHello.Random.
 *          -   The label is "master secret" or "extended master secret".
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for the PSK)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for the other inputs)
 */
#define PSA_ALG_TLS12_PSK_TO_MS(hash_alg) \
    ((psa_algorithm_t)(0x08000300 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   Macro to build a PBKDF2-HMAC password-hashing or key-stretching algorithm.
 *
 * @details PBKDF2 is specified by PKCS #5: Password-Based Cryptography Specification Version 2.1
 *          [RFC8018](https://tools.ietf.org/html/rfc8018.html) §5.2. This macro constructs a
 *          PBKDF2 algorithm that uses a pseudo-random function based on HMAC with the specified
 *          hash.
 *
 *          This key derivation algorithm uses the following inputs, which must be provided in the
 *          following order:
 *          -   @ref PSA_KEY_DERIVATION_INPUT_COST is the iteration count. This input step must be
 *              used exactly once.
 *          -   @ref PSA_KEY_DERIVATION_INPUT_SALT is the salt. This input step must be used one or
 *              more times; if used several times, the inputs will be concatenated. This can be
 *              used to build the final salt from multiple sources, both public and secret (also
 *              known as pepper).
 *          -   @ref PSA_KEY_DERIVATION_INPUT_PASSWORD is the password to be hashed. This input
 *              step must be used exactly once.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for password input)
 *          - @ref PSA_KEY_TYPE_PASSWORD (for password input)
 *          - @ref PSA_KEY_TYPE_PEPPER (for salt input)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for salt input)
 *          - @ref PSA_KEY_TYPE_PASSWORD_HASH (for key verification)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true.
 *
 * @return  The corresponding `PBKDF2-HMAC-XXX` algorithm. For example,
 *          @ref PSA_ALG_PBKDF2_HMAC(@ref PSA_ALG_SHA_256) is the algorithm identifier for
 *          `PBKDF2-HMAC-SHA-256`.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_PBKDF2_HMAC(hash_alg) \
    ((psa_algorithm_t)(0x08800100 | ((hash_alg) & 0x000000ff)))

/**
 * @brief   The PBKDF2-AES-CMAC-PRF-128 password-hashing or key-stretching algorithm.
 *
 * @details PBKDF2 is specified by PKCS #5: Password-Based Cryptography Specification Version 2.1
 *          [RFC8018](https://tools.ietf.org/html/rfc8018.html) §5.2. This algorithm specifies the
 *          PBKDF2 algorithm using the AES-CMAC-PRF-128 pseudo-random function specified by
 *          [RFC4615](https://tools.ietf.org/html/rfc4615.html).
 *
 *          This key derivation algorithm uses the same inputs as @ref PSA_ALG_PBKDF2_HMAC() with
 *          the same constraints.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_DERIVE (for password input)
 *          - @ref PSA_KEY_TYPE_PASSWORD (for password input)
 *          - @ref PSA_KEY_TYPE_PEPPER (for salt input)
 *          - @ref PSA_KEY_TYPE_RAW_DATA (for salt input)
 *          - @ref PSA_KEY_TYPE_PASSWORD_HASH (for key verification)
 */
#define PSA_ALG_PBKDF2_AES_CMAC_PRF_128 ((psa_algorithm_t)0x08800200)

#ifdef __cplusplus
}
#endif

/** @} */
