/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        psa_crypto_operation_encoder.h
 * @brief       Macros used to map PSA akgorithms, key types and key sizes to specific key types
 *              and operations to call the corresponding driver functions.
 *
 * @note        Currently this only supports a small number of operations. It should be expanded as
 *              needed as this implementation increases support for more operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_OPERATION_ENCODER_H
#define PSA_CRYPTO_OPERATION_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"
#include "psa_crypto_slot_management.h"

/**
 * @brief   Unknown or invalid operation
 */
#define PSA_INVALID_OPERATION     (0xFF)

/**
 * @brief   Enum encoding available cipher operations.
 *
 * @details To be expanded with the development of this implementation.
 */
typedef enum {
    PSA_CBC_NO_PAD_AES_128,
    PSA_CBC_NO_PAD_AES_192,
    PSA_CBC_NO_PAD_AES_256,
    PSA_CBC_PKCS7_AES_256
} psa_cipher_op_t;

/**
 * @brief   Enum encoding available asymmetric key types and sizes.
 *
 * @details To be expanded with the development of this implementation.
 */
typedef enum {
    PSA_ECC_P160_K1,
    PSA_ECC_P160_R1,
    PSA_ECC_P160_R2,
    PSA_ECC_P192_K1,
    PSA_ECC_P192_R1,
    PSA_ECC_P224_K1,
    PSA_ECC_P224_R1,
    PSA_ECC_P256_K1,
    PSA_ECC_P256_R1,
    PSA_ECC_P384_R1,
    PSA_ECC_P521_R1,
    PSA_ECC_FRP,
    PSA_ECMONT_255,
    PSA_ECMONT_448
} psa_asym_key_t;

/**
 * @brief   Combine an ECC 192 key type with a given curve.
 *
 * @param   curve   Must be a curve of type @ref psa_ecc_family_t
 *
 * @return  @ref psa_asym_key_t
 *          @ref PSA_INVALID_OPERATION  @c curve is not compatible with key size
 */
#define GET_ECC_KEY_TYPE_192(curve) \
    ((curve == PSA_ECC_FAMILY_SECP_R1) ? PSA_ECC_P192_R1 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Combine an ECC 265 key type with a given curve.
 *
 * @param   curve   Must be a curve of type @ref psa_ecc_family_t
 *
 * @return  @ref psa_asym_key_t
 *          @ref PSA_INVALID_OPERATION  @c curve is not compatible with key size
 */
#define GET_ECC_KEY_TYPE_256(curve) \
    ((curve == PSA_ECC_FAMILY_SECP_R1) ? PSA_ECC_P256_R1 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Map an ECC 192 key to a given curve according to its type and size.
 *
 * @param   bits    Key size of type @ref psa_key_bits_t
 * @param   curve   Must be a curve of type @ref psa_ecc_family_t
 *
 * @return  @ref psa_asym_key_t
 *          @ref PSA_INVALID_OPERATION  @c curve and @c bits are incompatible
 */
#define PSA_ENCODE_ECC_KEY_TYPE(bits, curve) \
    ((bits == 256) || (bits == 520) ? GET_ECC_KEY_TYPE_256(curve) : \
     (bits == 192) || (bits == 392) ? GET_ECC_KEY_TYPE_192(curve) : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Combine algorithm, and key type with a key size of 128 bits.
 *
 * @param   alg     Algorithm of type @ref psa_algorithm_t.
 * @param   type    Key type of type @ref psa_key_type_t
 *
 * @return  @ref psa_cipher_op_t
 *          @ref PSA_INVALID_OPERATION  @c alg, and @c type are incompatible with the key size
 */
#define GET_CIPHER_OPERATION_128(alg, type) \
    (((alg == PSA_ALG_CBC_NO_PADDING) && (type == PSA_KEY_TYPE_AES)) ? PSA_CBC_NO_PAD_AES_128 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Combine algorithm, and key type with a key size of 192 bits.
 *
 * @param   alg     Algorithm of type @ref psa_algorithm_t.
 * @param   type    Key type of type @ref psa_key_type_t
 *
 * @return  @ref psa_cipher_op_t
 *          @ref PSA_INVALID_OPERATION  @c alg, and @c type are incompatible with the key size
 */
#define GET_CIPHER_OPERATION_192(alg, type) \
    (((alg == PSA_ALG_CBC_NO_PADDING) && (type == PSA_KEY_TYPE_AES)) ? PSA_CBC_NO_PAD_AES_192 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Combine algorithm, and key type with a key size of 256 bits.
 *
 * @param   alg     Algorithm of type @ref psa_algorithm_t.
 * @param   type    Key type of type @ref psa_key_type_t
 *
 * @return  @ref psa_cipher_op_t
 *          @ref PSA_INVALID_OPERATION  @c alg, and @c type are incompatible with the key size
 */
#define GET_CIPHER_OPERATION_256(alg, type) \
    (((alg == PSA_ALG_CBC_NO_PADDING) && (type == PSA_KEY_TYPE_AES)) ? PSA_CBC_NO_PAD_AES_256 : \
     ((alg == PSA_ALG_CBC_PKCS7) && (type == PSA_KEY_TYPE_AES)) ? PSA_CBC_PKCS7_AES_256 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Map algorithm, key size and type to a specific operation.
 *
 * @param   alg     Algorithm of type @ref psa_algorithm_t.
 * @param   bits    Size of the used key of type @ref psa_key_bits_t
 * @param   type    Key type of type @ref psa_key_type_t
 *
 * @return  @ref psa_cipher_op_t
 *          @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define PSA_ENCODE_CIPHER_OPERATION(alg, bits, type) \
    ((bits == 128) ? GET_CIPHER_OPERATION_128(alg, type) : \
     (bits == 192) ? GET_CIPHER_OPERATION_192(alg, type) : \
     (bits == 256) ? GET_CIPHER_OPERATION_256(alg, type) : \
     PSA_INVALID_OPERATION)

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_OPERATION_ENCODER_H */
/** @} */
