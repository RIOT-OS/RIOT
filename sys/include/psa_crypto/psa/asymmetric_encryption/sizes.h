/*
 * Copyright (C) 2025 TU Dresden
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
 * @file
 * @brief       Asymmetric encryption size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A sufficient output buffer size for @ref psa_asymmetric_decrypt(),
 *          for any of the supported key types and asymmetric encryption algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_decrypt() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE().
 */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE \
/* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_asymmetric_decrypt().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_decrypt() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_ASYMMETRIC_DECRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    An asymmetric key type, either a key pair or a public key.
 * @param   key_bits    The size of the key in bits.
 * @param   alg         An asymmetric encryption algorithm: a value of type psa_algorithm_t such
 *                      that @ref PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(@p alg) is true.
 *
 * @return  A sufficient output buffer size for the specified asymmetric encryption algorithm
 *          and key parameters.
 *          0 if the asymmetric encryption algorithm and key parameters are not supported.
 *          Unspecified if the parameters are not valid.
 */
#define PSA_ASYMMETRIC_DECRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_asymmetric_encrypt(),
 *          for any of the supported key types and asymmetric encryption algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_encrypt() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE().
 */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_asymmetric_encrypt().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_asymmetric_encrypt() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_ASYMMETRIC_ENCRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    An asymmetric key type, either a key pair or a public key.
 * @param   key_bits    The size of the key in bits.
 * @param   alg         An asymmetric encryption algorithm: a value of type psa_algorithm_t
 *                      such that @ref PSA_ALG_IS_ASYMMETRIC_ENCRYPTION(@p alg) is true.
 *
 * @return  A sufficient output buffer size for the specified asymmetric encryption algorithm
 *          and key parameters.
 *          0 if the asymmetric encryption algorithm and key parameters are not supported.
 *          Unspecified if the parameters are not valid.
 */
#define PSA_ASYMMETRIC_ENCRYPT_OUTPUT_SIZE(key_type, key_bits, alg) \
/* implementation-defined value */

#ifdef __cplusplus
}
#endif

/** @} */
