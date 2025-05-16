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
 * @file        asymmetric_encryption/algorithm.h
 * @brief       Asymmetric encryption algorithm definitions for the PSA Crypto API
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
 * @brief   Category for asymmetric encryption algorithms
 */
#define PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION  ((psa_algorithm_t)0x07000000)

/**
 * @brief   Whether the specified algorithm is an asymmetric encryption algorithm, also known as
 *          public-key encryption algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is an asymmetric encryption algorithm,
 *          0 otherwise.
 */
#define PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(alg) \
    (((alg) & PSA_ALG_CATEGORY_MASK) == PSA_ALG_CATEGORY_ASYMMETRIC_ENCRYPTION)

/**
 * @brief   Whether the specified algorithm is an RSA OAEP encryption algorithm.
 *
 * @param   alg An algorithm identifier: a value of type @ref psa_algorithm_t.
 *
 * @return  1 if alg is a RSA OAEP algorithm
 *          0 otherwise
 */
#define PSA_ALG_IS_RSA_OAEP(alg) \
    (((alg) & ~0x000000ff) == 0x07000300)

/**
 * @brief   The RSA PKCS#1 v1.5 asymmetric encryption algorithm.
 *
 * @details This encryption scheme is defined by PKCS #1: RSA Cryptography Specifications
 *          Version 2.2 [RFC8017](https://tools.ietf.org/html/rfc8017.html) §7.2 under the
 *          name RSAES-PKCS-v1_5.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (asymmetric encryption only)
 */
#define PSA_ALG_RSA_PKCS1V15_CRYPT ((psa_algorithm_t)0x07000200)

/**
 * @brief   The RSA OAEP asymmetric encryption algorithm.
 *
 * @details This encryption scheme is defined by
 *          [RFC8017](https://tools.ietf.org/html/rfc8017.html) §7.1 under the name RSAES-OAEP,
 *          with the following options:
 *          -   The mask generation function MGF1 defined in
 *              [RFC8017](https://tools.ietf.org/html/rfc8017.html) Appendix B.2.1.
 *          -   The specified hash algorithm is used to hash the label, and for the mask generation
 *              function.
 *
 *          @b Compatible @b key @b types
 *          - @ref PSA_KEY_TYPE_RSA_KEY_PAIR
 *          - @ref PSA_KEY_TYPE_RSA_PUBLIC_KEY (asymmetric encryption only)
 *
 * @param   hash_alg    A hash algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_HASH(@p hash_alg) is true. The hash algorithm is used for
 *                      MGF1.
 *
 * @return  The corresponding RSA OAEP encryption algorithm.
 *          Unspecified if @c hash_alg is not a supported hash algorithm.
 */
#define PSA_ALG_RSA_OAEP(hash_alg) \
    ((psa_algorithm_t)(0x07000300 | ((hash_alg) & 0x000000ff)))

#ifdef __cplusplus
}
#endif

/** @} */
