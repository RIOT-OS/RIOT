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
 * @brief       Key size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/sizes.h"
#include "type.h"

/**
 * @brief   Maximum key size in bytes, determined by the build system.
 *
 * @details The maximum key size is set automatically, depending on
 *          the features chosen at compile-time. They should not be
 *          changed manually.
 */
#if   (IS_USED(MODULE_PSA_MAC_HMAC_SHA_256))
#define CONFIG_PSA_MAX_KEY_SIZE 64
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1) || \
       IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519) || \
       IS_USED(MODULE_PSA_CIPHER_AES_256_CBC) || \
       IS_USED(MODULE_PSA_AEAD_AES_256_CCM) || \
       IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256) || \
       IS_USED(MODULE_PSA_CIPHER_CHACHA20))
#define CONFIG_PSA_MAX_KEY_SIZE 32
#elif (IS_USED(MODULE_PSA_CIPHER_AES_192_CBC) || \
       IS_USED(MODULE_PSA_AEAD_AES_192_CCM) || \
       IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1))
#define CONFIG_PSA_MAX_KEY_SIZE 24
#elif (IS_USED(MODULE_PSA_CIPHER_AES_128_CBC)) || \
      (IS_USED(MODULE_PSA_AEAD_AES_128_CCM)) || \
      (IS_USED(MODULE_PSA_CIPHER_AES_128_ECB))
#define CONFIG_PSA_MAX_KEY_SIZE 16
#else
#define CONFIG_PSA_MAX_KEY_SIZE 0
#endif

/**
 * @brief   Maximum size of the export encoding of an ECC keypair.
 *
 * @details The representation of an ECC keypair follows
 *          https://arm-software.github.io/psa-api/crypto/1.1/api/keys/management.html#key-formats
 *          and is dependent on the family:
 *          - for twisted Edwards curves: 32B
 *          - for Weierstrass curves: `ceiling(m/8)`-byte string, big-endian
 *            where m is the bit size associated with the curve.
 */
#define PSA_KEY_EXPORT_ECC_KEY_MAX_SIZE(key_type, key_bits)                  \
    (size_t)\
    (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_TWISTED_EDWARDS ? 32 : \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_SECP_R1 ? \
     PSA_BITS_TO_BYTES(key_bits) : \
     0))

/**
 * @brief   Sufficient output buffer size for @ref psa_export_key().
 *
 * @details The following code illustrates how to allocate enough memory to export a key by
 *          querying the key type and size at runtime.
 *
 *          @code
 *          @ref psa_key_attributes_t attributes = @ref PSA_KEY_ATTRIBUTES_INIT;
 *          @ref psa_status_t status;
 *          status = @ref psa_get_key_attributes(key, &attributes);
 *          if (status != @ref PSA_SUCCESS)
 *              handle_error(...);
 *          @ref psa_key_type_t key_type = @ref psa_get_key_type(&attributes);
 *          size_t key_bits = @ref psa_get_key_bits(&attributes);
 *          size_t buffer_size = @ref PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits);
 *          @ref psa_reset_key_attributes(&attributes);
 *          uint8_t *buffer = malloc(buffer_size);
 *          if (buffer == NULL)
 *              handle_error(...);
 *          size_t buffer_length;
 *          status = @ref psa_export_key(key, buffer, buffer_size, &buffer_length);
 *          if (status != @ref PSA_SUCCESS)
 *              handle_error(...);
 *          @endcode
 *
 *          See also @ref PSA_EXPORT_KEY_PAIR_MAX_SIZE and @ref PSA_EXPORT_PUBLIC_KEY_MAX_SIZE.
 *
 * @param   key_type A supported key type.
 * @param   key_bits The size of the key in bits.
 *
 * @return  If the parameters are valid and supported, return a buffer size in bytes that
 *          guarantees that @ref psa_export_key() or @ref psa_export_public_key() will not fail
 *          with @ref PSA_ERROR_BUFFER_TOO_SMALL.
 *          0 if the parameters are a valid combination that is not supported by the implementation.
 *          Unspecified if the parameters are not valid.
 */
#define PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits) \
    (PSA_KEY_TYPE_IS_PUBLIC_KEY(key_type) ? \
     PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits) : \
     (PSA_KEY_TYPE_IS_ECC(key_type) ? \
      PSA_KEY_EXPORT_ECC_KEY_MAX_SIZE(key_type, key_bits) :   \
      0))

/**
 * @brief   Check whether the key size is a valid ECC size for key type.
 *
 * @param   type key type of of type @ref psa_key_type_t
 * @param   bits Key size of type @ref psa_key_bits_t
 */
#define PSA_ECC_KEY_SIZE_IS_VALID(type, bits) \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(type) == PSA_ECC_FAMILY_TWISTED_EDWARDS ?  \
        (bits == 255) :                                                     \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(type) == PSA_ECC_FAMILY_SECP_R1 ?          \
        (bits == 128 || \
         bits == 192 || \
         bits == 224 || \
         bits == 256 || \
         bits == 384) : \
    0))

/**
 * @brief   The maximum size of an asymmetric private key.
 */
#define PSA_MAX_PRIV_KEY_SIZE   (PSA_BYTES_TO_BITS(CONFIG_PSA_MAX_KEY_SIZE))

/**
 * @brief   Sufficient buffer size for exporting any asymmetric key pair.
 *
 * @details This value must be a sufficient buffer size when calling @ref psa_export_key() to
 *          export any asymmetric key pair that is supported by the implementation, regardless of
 *          the exact key type and key size.
 *
 *          See also @ref PSA_EXPORT_KEY_OUTPUT_SIZE().
 */
#if (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1) || \
     IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256))
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE \
    (PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_ECC_FAMILY_SECT_R1, 256))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519))
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE \
    (PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_ECC_FAMILY_TWISTED_EDWARDS, 255))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1))
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE \
    (PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_ECC_FAMILY_SECT_R1, 192))
#else
#define PSA_EXPORT_KEY_PAIR_MAX_SIZE 0
#endif

/**
 * @brief   Maximum size of the export encoding of an ECC public key.
 *
 * @details The representation of an ECC public key is dependent on the family:
 *          - for twisted Edwards curves: 32B
 *          - for Weierstrass curves:
 *            - The byte 0x04;
 *            - `x_P` as a `ceiling(m/8)`-byte string, big-endian;
 *            - `y_P` as a `ceiling(m/8)`-byte string, big-endian;
 *            - where m is the bit size associated with the curve.
 *            - 1 byte + 2 * point size.
 */
#define PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(key_type, key_bits)                  \
    (PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) == PSA_ECC_FAMILY_TWISTED_EDWARDS ? 32 : \
     ((size_t)(2 * PSA_BITS_TO_BYTES(key_bits) + 1)))

/**
 * @brief   Sufficient output buffer size for @ref psa_export_public_key().
 *
 * @details This macro returns a compile-time constant if its arguments are
 *          compile-time constants.
 *
 * @warning This macro may evaluate its arguments multiple times or
 *          zero times, so you should not pass arguments that contain
 *          side effects.
 *
 *          The following code illustrates how to allocate enough memory to export
 *          a public key by querying the key type and size at runtime.
 *
 *          @code
 *          @ref psa_key_attributes_t attributes = @ref PSA_KEY_ATTRIBUTES_INIT;
 *          @ref psa_status_t status;
 *          status = @ref psa_get_key_attributes(key, &attributes);
 *          if (status != @ref PSA_SUCCESS) handle_error(...);
 *          @ref psa_key_type_t key_type = @ref psa_get_key_type(&attributes);
 *          size_t key_bits = @ref psa_get_key_bits(&attributes);
 *          size_t buffer_size = @ref PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits);
 *          @ref psa_reset_key_attributes(&attributes);
 *          uint8_t *buffer = malloc(buffer_size);
 *          if (buffer == NULL) handle_error(...);
 *          size_t buffer_length;
 *          status = @ref psa_export_public_key(key, buffer, buffer_size, &buffer_length);
 *          if (status != @ref PSA_SUCCESS) handle_error(...);
 *          @endcode
 *
 * @param   key_type    A public key or key pair key type.
 * @param   key_bits    The size of the key in bits.
 *
 * @return  A buffer size in bytes that guarantees that @ref psa_export_public_key() will not fail
 *          with @ref PSA_ERROR_BUFFER_TOO_SMALL.
 *          0 if the parameters are a valid combination that is not supported.
 *          Unspecified if the parameters are not valid, the return value is unspecified.
 *          If the parameters are valid and supported, return the same result as
 *          @ref PSA_EXPORT_KEY_OUTPUT_SIZE(
 *          @ref PSA_KEY_TYPE_PUBLIC_KEY_OF_KEY_PAIR(@p key_type), @p key_bits).
 */
#define PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits)                           \
    (PSA_KEY_TYPE_IS_ECC(key_type) ? PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(key_type, key_bits) : \
     0)

/**
 * @brief   Sufficient buffer size for exporting any asymmetric public key.
 *
 * @details This macro expands to a compile-time constant integer. This value is
 *          a sufficient buffer size when calling @ref psa_export_key() or
 *          @ref psa_export_public_key() to export any asymmetric public key,
 *          regardless of the exact key type and key size.
 *
 *          See also @ref PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(@p key_type, @p key_bits).
 */
#if (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1) || \
     IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
    (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_ECC_FAMILY_SECT_R1, 256))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P192R1))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
    (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_ECC_FAMILY_SECT_R1, 192))
#elif (IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519))
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
    (PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(PSA_ECC_FAMILY_TWISTED_EDWARDS, 255))
#else
#define PSA_EXPORT_PUBLIC_KEY_MAX_SIZE 0
#endif

/**
 * @brief   The maximum size of an asymmetric private key buffer. If only a secure element driver is
 *          present, the private key will always be stored in a key slot and PSA Crypto will only
 *          allocate memory for an 8 Byte key slot number.
 */
#define PSA_MAX_PRIV_KEY_BUFFER_SIZE (PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE))

/**
 * @brief   The maximum size of an asymmetric private key pair.
 */
#define PSA_MAX_ASYMMETRIC_KEYPAIR_SIZE     (PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE) + \
                                             PSA_EXPORT_PUBLIC_KEY_MAX_SIZE)

/**
 * @brief   The maximum size of the used key data.
 */
#if IS_USED(MODULE_PSA_ASYMMETRIC)
#define PSA_MAX_KEY_DATA_SIZE  (PSA_EXPORT_PUBLIC_KEY_MAX_SIZE)
#else
#define PSA_MAX_KEY_DATA_SIZE  (CONFIG_PSA_MAX_KEY_SIZE)
#endif

/**
 * @brief   The maximum size of an unstructured key.
 */
#define PSA_MAX_UNSTRUCTURED_KEY_SIZE (CONFIG_PSA_MAX_KEY_SIZE)

#ifdef __cplusplus
}
#endif

/** @} */
