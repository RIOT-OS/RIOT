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
 * @brief       Asymmetric signature size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/sizes.h"
#include "psa/key/type.h"

/**
 * @brief   A sufficient signature buffer size for @ref psa_sign_message() and
 *          @ref psa_sign_hash(), for any of the supported key types and asymmetric signature
 *          algorithms.
 *
 * @details If the size of the signature buffer is at least this large, it is guaranteed that
 *          @ref psa_sign_message() and @ref psa_sign_hash() will not fail due to an insufficient
 *          buffer size.
 *
 *          See also @ref PSA_SIGN_OUTPUT_SIZE().
 */
#define PSA_SIGNATURE_MAX_SIZE /* implementation-defined value */

/**
 * @brief   ECDSA signature size for a given curve bit size
 *
 * @note    This macro returns a compile-time constant if its argument is one.
 *
 * @param   curve_bits    Curve size in bits.
 *
 * @return  Signature size in bytes.
 */
#define PSA_ECDSA_SIGNATURE_SIZE(curve_bits)    \
    ((size_t)(PSA_BITS_TO_BYTES(curve_bits) * 2))

/**
 * @brief   Sufficient signature buffer size for @ref psa_sign_message() and @ref psa_sign_hash().
 *
 * @details If the size of the signature buffer is at least this large, it is guaranteed that
 *          @ref psa_sign_message() and @ref psa_sign_hash() will not fail due to an insufficient
 *          buffer size. The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_SIGNATURE_MAX_SIZE.
 *
 * @param   key_type An asymmetric key type. This can be a key pair type or a public key type.
 * @param   key_bits The size of the key in bits.
 * @param   alg      The signature algorithm.
 *
 * @return  A sufficient signature buffer size for the specified asymmetric signature algorithm and
 *          key parameters.
 *          0 if algorithm and key parameters are not supported.
 *          If the parameters are not valid, the return value is unspecified.
 */
#define PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, alg)        \
    (PSA_KEY_TYPE_IS_ECC(key_type) ? PSA_ECDSA_SIGNATURE_SIZE(key_bits) : \
     ((void)alg, 0))

#ifdef __cplusplus
}
#endif

/** @} */
