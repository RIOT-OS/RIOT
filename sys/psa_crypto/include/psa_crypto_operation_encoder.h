/*
 * Copyright (C) 2022 HAW Hamburg
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
 * @file        psa_crypto_operation_encoder.h
 * @brief       Macros used to map PSA algorithms, key types and key sizes to specific key types
 *              and operations to call the corresponding driver functions.
 *
 * @note        Currently this only supports a small number of operations. It should be expanded as
 *              needed as this implementation increases support for more operations.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

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
    PSA_CBC_PKCS7_AES_256,
    PSA_STREAM_CIPHER_CHACHA20
} psa_cipher_op_t;

/**
 * @brief   Enum encoding available aead operations.
 *
 * @details To be expanded with the development of this implementation.
 */
typedef enum {
    PSA_CCM_AES_128,
    PSA_CCM_AES_192,
    PSA_CCM_AES_256
} psa_aead_op_t;

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
    PSA_ECC_ED25519,
} psa_asym_key_t;

/**
 * @brief   Combine a SECP_R1 key type with a given key size (private or public key).
 *
 * @param   bits    Key size of type @ref psa_key_bits_t
 *
 * @return  @ref psa_asym_key_t
 * @return  @ref PSA_INVALID_OPERATION  @c bits is not compatible with SECP_R1 curves
 */
#define PSA_ENCODE_ECC_KEY_TYPE_SECPR1(bits) \
    ((bits == 256) || (bits == 520) ? PSA_ECC_P256_R1 : \
     (bits == 192) || (bits == 392) ? PSA_ECC_P192_R1 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Combine a Twisted Edwards key type with a given key size (private or public key).
 *
 * @param   bits    Key size of type @ref psa_key_bits_t
 *
 * @return  @ref psa_asym_key_t
 * @return  @ref PSA_INVALID_OPERATION  @c bits is not compatible with Twisted Edwards curves
 */
#define PSA_ENCODE_ECC_KEY_TYPE_EDWARDS(bits) \
    ((bits == 255) || (bits == 256) ? PSA_ECC_ED25519 : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Map an ECC key to a given curve according to its type and size.
 *
 * @param   bits    Key size of type @ref psa_key_bits_t
 * @param   curve   Must be a curve of type @ref psa_ecc_family_t
 *
 * @return  @ref psa_asym_key_t
 * @return  @ref PSA_INVALID_OPERATION  @c curve and @c bits are incompatible
 */
#define PSA_ENCODE_ECC_KEY_TYPE(bits, curve) \
    ((curve == PSA_ECC_FAMILY_SECP_R1) ? PSA_ENCODE_ECC_KEY_TYPE_SECPR1(bits) : \
     (curve == PSA_ECC_FAMILY_TWISTED_EDWARDS) ? PSA_ENCODE_ECC_KEY_TYPE_EDWARDS(bits) : \
     PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_CBC_NO_PADDING algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_CBC_NO_PADDING_OPERATION(type, bits) \
    ((type == PSA_KEY_TYPE_AES) ? \
     ((bits == 128) ? PSA_CBC_NO_PAD_AES_128 : \
     (bits == 192) ? PSA_CBC_NO_PAD_AES_192 : \
     (bits == 256) ? PSA_CBC_NO_PAD_AES_256 : \
     PSA_INVALID_OPERATION) : \
     PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_CCM algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_aead_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_AES_CCM_OPERATION(type, bits) \
    ((type == PSA_KEY_TYPE_AES) ? \
     ((bits == 128) ? PSA_CCM_AES_128 : \
     (bits == 192) ? PSA_CCM_AES_192 : \
     (bits == 256) ? PSA_CCM_AES_256 : \
     PSA_INVALID_OPERATION) : \
     PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_CBC_PKCS7 algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_CBC_PKCS7_OPERATION(type, bits) \
    (((alg == PSA_ALG_CBC_PKCS7) && (type == PSA_KEY_TYPE_AES)) ? PSA_CBC_PKCS7_AES_256 : \
     PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_CFB algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_CFB_OPERATION(type, bits) \
    (PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_CTR algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_CTR_OPERATION(type, bits) \
    (PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_ECB_NO_PADDING algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_ECB_NO_PADDING_OPERATION(type, bits) \
    (PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_OFB algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_OFB_OPERATION(type, bits) \
    (PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_STREAM_CIPHER algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_STREAM_CIPHER_OPERATION(type, bits) \
    (((type == PSA_KEY_TYPE_CHACHA20) && (bits == 256)) ? PSA_STREAM_CIPHER_CHACHA20 : \
     PSA_INVALID_OPERATION)

/**
 * @brief Combine key type and size with a PSA_ALG_XTS algorithm
 *
 * @param type Key type of type @ref psa_key_type_t
 * @param bits Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define GET_XTS_OPERATION(type, bits) \
    (PSA_INVALID_OPERATION)

/**
 * @brief   Map algorithm, key size and type to a specific operation.
 *
 * @param   alg     Algorithm of type @ref psa_algorithm_t.
 * @param   type    Key type of type @ref psa_key_type_t
 * @param   bits    Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_cipher_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define PSA_ENCODE_CIPHER_OPERATION(alg, type, bits) \
    ((alg == PSA_ALG_CBC_NO_PADDING) ? GET_CBC_NO_PADDING_OPERATION(type, bits) : \
     (alg == PSA_ALG_CBC_PKCS7) ? GET_CBC_PKCS7_OPERATION(type, bits) : \
     (alg == PSA_ALG_CFB) ? GET_CFB_OPERATION(type, bits) : \
     (alg == PSA_ALG_CTR) ? GET_CTR_OPERATION(type, bits) : \
     (alg == PSA_ALG_ECB_NO_PADDING) ? GET_ECB_NO_PADDING_OPERATION(type, bits) : \
     (alg == PSA_ALG_OFB) ? GET_OFB_OPERATION(type, bits) : \
     (alg == PSA_ALG_STREAM_CIPHER) ? GET_STREAM_CIPHER_OPERATION(type, bits) : \
     (alg == PSA_ALG_XTS) ? GET_XTS_OPERATION(type, bits) : \
     PSA_INVALID_OPERATION)

/**
 * @brief   Map algorithm, key size and type to a specific operation.
 *
 * @param   alg     Algorithm of type @ref psa_algorithm_t.
 * @param   type    Key type of type @ref psa_key_type_t
 * @param   bits    Size of the used key of type @ref psa_key_bits_t
 *
 * @return  @ref psa_aead_op_t
 * @return  @ref PSA_INVALID_OPERATION  @c alg, @c bits and @c type are not compatible
 */
#define PSA_ENCODE_AEAD_OPERATION(alg, type, bits) \
    ((PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg) == PSA_ALG_CCM) ? \
        GET_AES_CCM_OPERATION(type, bits) : PSA_INVALID_OPERATION)

#ifdef __cplusplus
}
#endif

/** @} */
