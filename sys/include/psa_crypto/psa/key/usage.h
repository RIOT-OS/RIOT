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
 * @file        key/usage.h
 * @brief       Key usage definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Encoding of permitted usage on a key.
 */
typedef uint32_t psa_key_usage_t;

/**
 * @brief   Permission to export the key.
 *
 * @details This flag allows the use of @ref psa_export_key() to export a key from the
 *          cryptoprocessor. A public key or the public part of a key pair can always be exported
 *          regardless of the value of this permission flag.
 *
 *          This flag can also be required to copy a key using @ref psa_copy_key() outside of a
 *          secure element. See also @ref PSA_KEY_USAGE_COPY.
 *
 *          If a key does not have export permission, implementations must not allow the key to be
 *          exported in plain form from the cryptoprocessor, whether through @ref psa_export_key()
 *          or through a proprietary interface. The key might still be exportable in a wrapped
 *          form, i.e. in a form where it is encrypted by another key.
 */
#define PSA_KEY_USAGE_EXPORT ((psa_key_usage_t)0x00000001)

/**
 * @brief   Permission to copy the key.
 *
 * @details This flag allows the use of @ref psa_copy_key() to make a copy of the key with the same
 *          policy or a more restrictive policy.
 *
 *          For lifetimes for which the key is located in a secure element which enforce the
 *          non-exportability of keys, copying a key outside the secure element also requires the
 *          usage flag @ref PSA_KEY_USAGE_EXPORT. Copying the key inside the secure element is
 *          permitted with just @ref PSA_KEY_USAGE_COPY if the secure element supports it. For keys
 *          with the lifetime @ref PSA_KEY_LIFETIME_VOLATILE or @ref PSA_KEY_LIFETIME_PERSISTENT,
 *          the usage flag @ref PSA_KEY_USAGE_COPY is sufficient to permit the copy.
 */
#define PSA_KEY_USAGE_COPY ((psa_key_usage_t)0x00000002)

/**
 * @brief   Permission for the implementation to cache the key.
 *
 * @details This flag allows the implementation to make additional copies of the key material that
 *          are not in storage and not for the purpose of an ongoing operation. Applications can
 *          use it as a hint to keep the key around for repeated access.
 *
 *          An application can request that cached key material is removed from memory by calling
 *          @ref psa_purge_key().
 *
 *          The presence of this usage flag when creating a key is a hint:
 *          - An implementation is not required to cache keys that have this usage flag.
 *          - An implementation must not report an error if it does not cache keys.
 *
 *          If this usage flag is not present, the implementation must ensure key material is
 *          removed from memory as soon as it is not required for an operation or for maintenance
 *          of a volatile key.
 *
 *          This flag must be preserved when reading back the attributes for all keys, regardless
 *          of key type or implementation behavior.
 */
#define PSA_KEY_USAGE_CACHE ((psa_key_usage_t)0x00000004)

/**
 * @brief   Permission to encrypt a message with the key.
 *
 * @details This flag allows the key to be used for a symmetric encryption operation, for an AEAD
 *          encryption-and-authentication operation, or for an asymmetric encryption operation, if
 *          otherwise permitted by the key’s type and policy. The flag must be present on keys used
 *          with the following APIs:
 *          - @ref psa_cipher_encrypt()
 *          - @ref psa_cipher_encrypt_setup()
 *          - @ref psa_aead_encrypt()
 *          - @ref psa_aead_encrypt_setup()
 *          - @ref psa_asymmetric_encrypt()
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_ENCRYPT ((psa_key_usage_t)0x00000100)

/**
 * @brief   Permission to decrypt a message with the key.
 *
 * @details This flag allows the key to be used for a symmetric decryption operation, for an AEAD
 *          decryption-and-verification operation, or for an asymmetric decryption operation, if
 *          otherwise permitted by the key’s type and policy. The flag must be present on keys used
 *          with the following APIs:
 *          - @ref psa_cipher_decrypt()
 *          - @ref psa_cipher_decrypt_setup()
 *          - @ref psa_aead_decrypt()
 *          - @ref psa_aead_decrypt_setup()
 *          - @ref psa_asymmetric_decrypt()
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_DECRYPT ((psa_key_usage_t)0x00000200)

/**
 * @brief   Permission to sign a message with the key.
 *
 * @details This flag allows the key to be used for a MAC calculation operation or for an
 *          asymmetric message signature operation, if otherwise permitted by the key’s type and
 *          policy. The flag must be present on keys used with the following APIs:
 *          - @ref psa_mac_compute()
 *          - @ref psa_mac_sign_setup()
 *          - @ref psa_sign_message()
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_SIGN_MESSAGE ((psa_key_usage_t)0x00000400)

/**
 * @brief   Permission to verify a message signature with the key.
 *
 * @details This flag allows the key to be used for a MAC verification operation or for an
 *          asymmetric message signature verification operation, if otherwise permitted by the
 *          key’s type and policy. The flag must be present on keys used with the following APIs:
 *          - @ref psa_mac_verify()
 *          - @ref psa_mac_verify_setup()
 *          - @ref psa_verify_message()
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_VERIFY_MESSAGE ((psa_key_usage_t)0x00000800)

/**
 * @brief   Permission to sign a message hash with the key.
 *
 * @details This flag allows the key to be used to sign a message hash as part of an asymmetric
 *          signature operation, if otherwise permitted by the key’s type and policy. The flag must
 *          be present on keys used when calling @ref psa_sign_hash().
 *
 *          This flag automatically sets @ref PSA_KEY_USAGE_SIGN_MESSAGE : if an application sets
 *          the flag @ref PSA_KEY_USAGE_SIGN_HASH when creating a key, then the key always has the
 *          permissions conveyed by @ref PSA_KEY_USAGE_SIGN_MESSAGE, and the flag
 *          @ref PSA_KEY_USAGE_SIGN_MESSAGE will also be present when the application queries the
 *          usage flags of the key.
 *
 *          For a key pair, this concerns the private key.
 */
#define PSA_KEY_USAGE_SIGN_HASH ((psa_key_usage_t)0x00001000)

/**
 * @brief   Permission to verify a message hash with the key.
 *
 * @details This flag allows the key to be used to verify a message hash as part of an asymmetric
 *          signature verification operation, if otherwise permitted by the key’s type and policy.
 *          The flag must be present on keys used when calling @ref psa_verify_hash().
 *
 *          This flag automatically sets @ref PSA_KEY_USAGE_VERIFY_MESSAGE : if an application sets
 *          the flag @ref PSA_KEY_USAGE_VERIFY_HASH when creating a key, then the key always has
 *          the permissions conveyed by @ref PSA_KEY_USAGE_VERIFY_MESSAGE, and the flag
 *          @ref PSA_KEY_USAGE_VERIFY_MESSAGE will also be present when the application queries the
 *          usage flags of the key.
 *
 *          For a key pair, this concerns the public key.
 */
#define PSA_KEY_USAGE_VERIFY_HASH ((psa_key_usage_t)0x00002000)

/**
 * @brief   Permission to derive other keys or produce a password hash from this key.
 *
 * @details This flag allows the key to be used for a key derivation operation or for a key
 *          agreement operation, if otherwise permitted by the key’s type and policy.
 *
 *          This flag must be present on keys used with the following APIs:
 *          - @ref psa_key_derivation_key_agreement()
 *          - @ref psa_raw_key_agreement()
 *
 *          If this flag is present on all keys used in calls to
 *          @ref psa_key_derivation_input_key() for a key derivation operation, then it permits
 *          calling @ref psa_key_derivation_output_bytes() or @ref psa_key_derivation_output_key()
 *          at the end of the operation.
 */
#define PSA_KEY_USAGE_DERIVE ((psa_key_usage_t)0x00004000)

/**
 * @brief   Permission to verify the result of a key derivation, including password hashing.
 *
 * @details This flag allows the key to be used in a key derivation operation, if otherwise
 *          permitted by the key’s type and policy.
 *
 *          This flag must be present on keys used with @ref psa_key_derivation_verify_key().
 *
 *          If this flag is present on all keys used in calls to
 *          @ref psa_key_derivation_input_key() for a key derivation operation, then it permits
 *          calling @ref psa_key_derivation_verify_bytes() or @ref psa_key_derivation_verify_key()
 *          at the end of the operation.
 */
#define PSA_KEY_USAGE_VERIFY_DERIVATION ((psa_key_usage_t)0x00008000)

#ifdef __cplusplus
}
#endif

/** @} */
