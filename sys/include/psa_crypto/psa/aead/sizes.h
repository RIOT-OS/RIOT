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
 * @brief       AEAD size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "algorithm.h"

/**
 * @brief   A sufficient plaintext buffer size for @ref psa_aead_decrypt(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the plaintext buffer is at least this large,
 *          it is guaranteed that @ref psa_aead_decrypt() will not fail due
 *          to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_DECRYPT_OUTPUT_SIZE().
 *
 * @param   ciphertext_length Size of the ciphertext in bytes.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE(ciphertext_length) \
/* implementation-defined value */

/**
 * @brief   The length of a tag for an AEAD algorithm, in bytes.
 *
 * @details This is the size of the tag output from @ref psa_aead_finish().
 *          If the size of the tag buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient tag buffer size.
 *
 *          See also @ref PSA_AEAD_TAG_MAX_SIZE.
 *
 * @param   key_type    The type of the AEAD key.
 * @param   key_bits    The size of the AEAD key in bits.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  The tag length for the specified algorithm and key.
 *          0 if the AEAD algorithm does not have an identified tag that can be distinguished from
 *          the rest of the ciphertext.
 *          0 if the AEAD algorithm is not recognized or not supported.
 */
#define PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg)     \
    (PSA_ALG_IS_AEAD(alg) ?                              \
    (((alg) & 0x003f0000) >> 16) :                       \
    ((void) (key_type), (void) (key_bits), 0))

/**
 * @brief   A sufficient buffer size for storing the tag output by @ref psa_aead_finish(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the tag buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_TAG_LENGTH().
 */
#define PSA_AEAD_TAG_MAX_SIZE (16)

/**
 * @brief   A sufficient buffer size for storing the tag output by @ref psa_aead_finish(),
 *          for AES key types and CCM algorithms.
 *
 * @details If the size of the tag buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_TAG_LENGTH().
 */
#define PSA_AES_CCM_TAG_MAX_SIZE (16)

/**
 * @brief   A sufficient plaintext buffer size for @ref psa_aead_decrypt(), in bytes.
 *
 * @details If the size of the plaintext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_decrypt() will not fail due to an insufficient buffer size. Depending on
 *          the algorithm, the actual size of the plaintext might be smaller.
 *
 *          See also @ref PSA_AEAD_DECRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type            A symmetric key type that is compatible with algorithm alg.
 * @param   alg                 An AEAD algorithm: a value of type @ref psa_algorithm_t
 *                              such that @ref PSA_ALG_IS_AEAD(@p alg) is true.
 * @param   ciphertext_length   Size of the ciphertext in bytes.
 *
 * @return  The AEAD plaintext size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_DECRYPT_OUTPUT_SIZE(key_type, alg, ciphertext_length) \
        (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 &&                  \
            ((ciphertext_length) > PSA_AEAD_TAG_LENGTH(key_type, 0, alg)) ?   \
        (ciphertext_length) - PSA_AEAD_TAG_LENGTH(key_type, 0, alg) : 0)

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_encrypt(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the ciphertext buffer is at least this large,
 *          it is guaranteed that @ref psa_aead_encrypt() will not fail due to an insufficient
 *          buffer size.
 *
 *          See also @ref PSA_AEAD_ENCRYPT_OUTPUT_SIZE().
 *
 * @param   plaintext_length Size of the plaintext in bytes.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(plaintext_length) \
       ((plaintext_length) + PSA_AEAD_TAG_MAX_SIZE)

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_encrypt(), in bytes.
 *
 * @details If the size of the ciphertext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_encrypt() will not fail due to an insufficient buffer size. Depending on
 *          the algorithm, the actual size of the ciphertext might be smaller.
 *
 *          See also @ref PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type            A symmetric key type that is compatible with algorithm alg.
 * @param   alg                 An AEAD algorithm: a value of type @ref psa_algorithm_t such
 *                              that @ref PSA_ALG_IS_AEAD(alg) is true.
 * @param   plaintext_length    Size of the plaintext in bytes.
 *
 * @return  The AEAD ciphertext size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_ENCRYPT_OUTPUT_SIZE(key_type, alg, plaintext_length) \
        (PSA_AEAD_NONCE_LENGTH(key_type, alg) != 0 ?                  \
        (plaintext_length) + PSA_AEAD_TAG_LENGTH(key_type, 0, alg) : 0)

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_finish(),
 *          for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the ciphertext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient ciphertext buffer size.
 *
 *          See also @ref PSA_AEAD_FINISH_OUTPUT_SIZE().
 */
#define PSA_AEAD_FINISH_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient ciphertext buffer size for @ref psa_aead_finish().
 *
 * @details If the size of the ciphertext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_finish() will not fail due to an insufficient ciphertext buffer size. The
 *          actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_AEAD_FINISH_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  A sufficient ciphertext buffer size for the specified key type and algorithm.
 *          If the key type or AEAD algorithm is not recognized, or the parameters are incompatible,
 *          return 0. An implementation can return either 0 or a correct size for a key type and
 *          AEAD algorithm that it recognizes, but does not support.
 */
#define PSA_AEAD_FINISH_OUTPUT_SIZE(key_type, alg) \
/* implementation-defined value */

/**
 * @brief   The default nonce size for an AEAD algorithm, in bytes.
 *
 * @details If the size of the nonce buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_generate_nonce() will not fail due to an insufficient buffer size.
 *
 *          For most AEAD algorithms, @ref PSA_AEAD_NONCE_LENGTH() evaluates to the exact size of
 *          the nonce generated by @ref psa_aead_generate_nonce().
 *
 *          See also @ref PSA_AEAD_NONCE_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  The default nonce size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_NONCE_LENGTH(key_type, alg) \
    ((PSA_BLOCK_CIPHER_BLOCK_LENGTH(key_type) == 16 && \
        ((PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_CCM) || \
        (PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_CCM))) || \
        (key_type == PSA_KEY_TYPE_CHACHA20 && \
        PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_CHACHA20_POLY1305) ? \
        12 : 0)

/**
 * @brief   A sufficient buffer size for storing the nonce generated by
 *          @ref psa_aead_generate_nonce(), for any of the supported key types and AEAD algorithms.
 *
 * @details If the size of the nonce buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_generate_nonce() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_NONCE_LENGTH().
 */
#define PSA_AEAD_NONCE_MAX_SIZE (13)

/**
 * @brief   A sufficient output buffer size for @ref psa_aead_update(), for any of the supported key
 *          types and AEAD algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_update() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_UPDATE_OUTPUT_SIZE().
 *
 * @param   input_length Size of the input in bytes.
 */
#define PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE(input_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_aead_update().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_update() will not fail due to an insufficient buffer size. The actual
 *          size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_AEAD_UPDATE_OUTPUT_MAX_SIZE.
 *
 * @param   key_type        A symmetric key type that is compatible with algorithm alg.
 * @param   alg             An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                          @ref PSA_ALG_IS_AEAD(@p alg) is true.
 * @param   input_length    Size of the input in bytes.
 *
 * @return  A sufficient output buffer size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_UPDATE_OUTPUT_SIZE(key_type, alg, input_length) \
/* implementation-defined value */

/**
 * @brief   A sufficient output buffer size for @ref psa_aead_update(), for any of the supported key
 *          types and AEAD algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_update() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_AEAD_UPDATE_OUTPUT_SIZE().
 *
 * @param   input_length Size of the input in bytes.
 */
#define PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief   A sufficient plaintext buffer size for @ref psa_aead_verify(), in bytes.
 *
 * @details If the size of the plaintext buffer is at least this large, it is guaranteed that
 *          @ref psa_aead_verify() will not fail due to an insufficient plaintext buffer size. The
 *          actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_AEAD_VERIFY_OUTPUT_MAX_SIZE.
 *
 * @param   key_type    A symmetric key type that is compatible with algorithm alg.
 * @param   alg         An AEAD algorithm: a value of type @ref psa_algorithm_t such that
 *                      @ref PSA_ALG_IS_AEAD(@p alg) is true.
 *
 * @return  A sufficient plaintext buffer size for the specified key type and algorithm.
 *          0 if the key type or AEAD algorithm is not recognized, not supported or the parameters
 *          are incompatible.
 */
#define PSA_AEAD_VERIFY_OUTPUT_SIZE(key_type, alg) \
/* implementation-defined value */

#ifdef __cplusplus
}
#endif

/** @} */
